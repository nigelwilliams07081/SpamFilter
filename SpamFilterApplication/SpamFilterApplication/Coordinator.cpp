#include "stdafx.h"

#define RANK_COORDINATOR 0

int Coordinator::m_emailsSent      = 0;
int Coordinator::m_totalEmails     = 0;
int Coordinator::m_repliesReceived = 0;
int Coordinator::m_activeWorkers   = 0;

EmailReader Coordinator::reader;

Coordinator::Coordinator() {
}

void Coordinator::mainLoop(const char* emailsource) {
	
	// Load emails from the source file
	// Send the OK if the file checks out
	int OK;
	
	try {
		printf("Loading XML data\n");
		reader.loadFromFile(emailsource);
		
		m_totalEmails = reader.getEmailCount();
		
		// Everything checks out
		printf("Success: loaded %i emails\n", m_totalEmails);
	
		OK = (int)true;
	} catch (const std::exception &e) {
		// Some exception from the XML parser came up
		// Perhaps the XML was malformed, the file could not be opened at all, etc
		OK = (int)false;
	}
	
	// Broadcast the OK to all nodes
	MPI::COMM_WORLD.Bcast(&OK, 1, MPI::INT, RANK_COORDINATOR);
	
	if (!OK) {
		printf("Failure. Exiting\n");
		return;
	}
		
	// Start up a thread to receive spam anaylsis results
	#ifndef SINGLETHREADED
	std::thread worker(receiveResult);
	#endif
	
	// Get the number of other nodes
	m_activeWorkers = MPI::COMM_WORLD.Get_size();
	m_activeWorkers--; // Minus the coordinator
	
	// Wait for threads to ask for a quantity of emails
	while (m_activeWorkers > 0) {
		printf("Waiting for worker node...\n");
		
		MPI::Status status;
		MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, TAG_EMAILS_REQUEST, status);
		
		// We recieved a request for emails, spawn a new thread to serve it
		printf("Detected a data request from node #%i\n", status.Get_source());
		
		#ifdef SINGLETHREADED
		talkWithNode(status.Get_source());
		#else
		std::thread(talkWithNode, status.Get_source()).detach();
		#endif
	}
	
	// Wait for the worker to finish
	#ifdef SINGLETHREADED
	receiveResult();
	#else
	worker.join();
	#endif
	
	return;
}

void Coordinator::talkWithNode(int nodeId) {
	
	// Receive quantity of emails to send
	int quantity;
	MPI::COMM_WORLD.Recv(&quantity, 1, MPI::INT, nodeId, TAG_EMAILS_REQUEST);
	
	printf("Received request from node #%i for %i emails\n", nodeId, quantity);
	
	// Calculate how many emails are left, and then use the smaller quantity
	int emailsRemaining = m_totalEmails - m_emailsSent;
	int sendingQuantity;
	
	if (emailsRemaining < quantity) {
		sendingQuantity = emailsRemaining;
	} else {
		sendingQuantity = quantity;
	}
	
	// Tell the worker node how many emails to expect
	MPI::COMM_WORLD.Send(&sendingQuantity, 1, MPI::INT, nodeId, TAG_EMAIL_QUANTITY);
	
	// If there are no more emails to send, exit
	if (sendingQuantity == 0) {
		printf("Sent termination signal to node #%i\n", nodeId);
		m_activeWorkers--;
		return;
	}	
	
	// Otherwise we send them all
	printf("Will send %i emails to node #%i\n", sendingQuantity, nodeId);
		
	// Set this now so other threads will not compete with this one
	m_emailsSent += sendingQuantity;
	
	printf("Sending emails...\n");
	
	for (int i = 0; i < sendingQuantity; i++) {
		// The worker node is entitled to emails number (emailsSent) till (emailSent + sendingQuantity)
		// No other nodes should be fighting the node over this
		Email e = reader.get(m_emailsSent - i);
		
		MPI_Send_string(e.Sender,  nodeId, TAG_EMAIL_SENDER);
		MPI_Send_string(e.Subject, nodeId, TAG_EMAIL_SUBJECT);
		MPI_Send_string(e.Body,    nodeId, TAG_EMAIL_BODY);
		
		MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::INT, nodeId, TAG_EMAIL_NUM_ATTACHMENTS);
		
		for (int i = 0; i < e.NumAttachments; i++) {
			MPI_Send_string(e.Attachments[i], nodeId, TAG_EMAIL_ATTACHMENT);
		}
		
		printf("Successfully sent email to node #%i\n", nodeId);
	}
	
	printf("Successfully sent all %i emails to node #%i\n", sendingQuantity, nodeId);
	
	return;
}

void Coordinator::receiveResult() {
	
	while (m_repliesReceived < m_totalEmails) {
		
		Email result;
		MPI::Status status;
		
		// Receive the Sender (first data point) from any source, then follow up with the rest from the same source only
		result.Sender  = MPI_Recv_string(MPI::ANY_SOURCE,   TAG_RETURN_EMAIL_SENDER, status);
		
		int node = status.Get_source();
		
		result.Subject = MPI_Recv_string(node, TAG_RETURN_EMAIL_SUBJECT);
		result.Body    = MPI_Recv_string(node, TAG_RETURN_EMAIL_BODY);
		
		float spamPercent;
		MPI::COMM_WORLD.Recv(&(result.NumAttachments), 1, MPI::INT,   node, TAG_RETURN_EMAIL_NUM_ATTACHMENTS);
		MPI::COMM_WORLD.Recv(&spamPercent,             1, MPI::FLOAT, node, TAG_RETURN_EMAIL_SPAM_PERCENTAGE);
		result.SpamPercentage = spamPercent;
		
		result.Attachments = new std::string[result.NumAttachments];
		for (int i = 0; i < result.NumAttachments; i++) {
			result.Attachments[i] = MPI_Recv_string(node, TAG_RETURN_EMAIL_ATTACHMENT);
		}
		
		m_repliesReceived++;
		
		printf("Analyzed email recieved from node!\n");
		// Result now contains an email that has been analyzed for spam
		// Do something with it here
	}
	
	return;
}