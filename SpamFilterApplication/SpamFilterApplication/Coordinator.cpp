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
	MPI_Bcast(&OK, sizeof(int), MPI_INT, RANK_COORDINATOR, MPI_COMM_WORLD);
	
	if (!OK) {
		printf("Failure. Exiting\n");
		return;
	}
		
	// Start up a thread to receive spam anaylsis results
	std::thread worker(receiveResult);
	
	// Get the number of other nodes
	MPI_Comm_size(MPI_COMM_WORLD, &m_activeWorkers);
	m_activeWorkers--; // Minus the coordinator
	
	// Wait for threads to ask for a quantity of emails
	while (m_activeWorkers > 0) {
		printf("Waiting for worker node...\n");
		
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, TAG_EMAILS_REQUEST, MPI_COMM_WORLD, &status);
		
		// We recieved a request for emails, spawn a new thread to serve it
		printf("Detected a data request from node #%i\n", status.MPI_SOURCE);
		std::thread(talkWithNode, status.MPI_SOURCE).detach();
	}
	
	// Wait for the worker to finish
	worker.join();
	
	return;
}

void Coordinator::talkWithNode(int nodeId) {
	
	// Receive quantity of emails to send
	int quantity;
	MPI_Recv(&quantity, 1, MPI_INT, nodeId, TAG_EMAILS_REQUEST, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
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
	MPI_Send(&sendingQuantity, 1, MPI_INT, nodeId, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD);
	
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
		
		MPI_Send_string(e.Sender,  nodeId, TAG_EMAIL_SENDER,  MPI_COMM_WORLD);
		MPI_Send_string(e.Subject, nodeId, TAG_EMAIL_SUBJECT, MPI_COMM_WORLD);
		MPI_Send_string(e.Body,    nodeId, TAG_EMAIL_BODY,    MPI_COMM_WORLD);
		
		MPI_Send(&(e.NumAttachments), 1, MPI_INT, nodeId, TAG_EMAIL_NUM_ATTACHMENTS, MPI_COMM_WORLD);
		
		for (int i = 0; i < e.NumAttachments; i++) {
			MPI_Send_string(e.Attachments[i], nodeId, TAG_EMAIL_ATTACHMENT, MPI_COMM_WORLD);
		}
		
		printf("Successfully sent email to node #%i\n", nodeId);
	}
	
	printf("Successfully sent all %i emails to node #%i\n", sendingQuantity, nodeId);
	
	return;
}

void Coordinator::receiveResult() {
	
	while (m_repliesReceived < m_totalEmails) {
		
		Email result;
		MPI_Status status;
		
		// Receive the Sender (first data point) from any source, then follow up with the rest from the same source only
		result.Sender  = MPI_Recv_string(MPI_ANY_SOURCE,    TAG_RETURN_EMAIL_SENDER,  MPI_COMM_WORLD, &status);
		result.Subject = MPI_Recv_string(status.MPI_SOURCE, TAG_RETURN_EMAIL_SUBJECT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		result.Body    = MPI_Recv_string(status.MPI_SOURCE, TAG_RETURN_EMAIL_BODY,    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		result.NumAttachments;
		MPI_Recv(&(result.NumAttachments), 1, MPI_INT, status.MPI_SOURCE, TAG_RETURN_EMAIL_NUM_ATTACHMENTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		result.Attachments = new std::string[result.NumAttachments];
		for (int i = 0; i < result.NumAttachments; i++) {
			result.Attachments[i] = MPI_Recv_string(status.MPI_SOURCE, TAG_RETURN_EMAIL_ATTACHMENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
		float spamPercent;
		MPI_Recv(&spamPercent, 1, MPI_FLOAT, status.MPI_SOURCE, TAG_RETURN_EMAIL_SPAM_PERCENTAGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		result.SpamPercentage = spamPercent;
		
		m_repliesReceived++;
		
		printf("Analyzed email recieved from node!\n");
		// Result now contains an email that has been analyzed for spam
		// Do something with it here
	}
	
	return;
}