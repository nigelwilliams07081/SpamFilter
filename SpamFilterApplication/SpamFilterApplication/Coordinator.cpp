#include "stdafx.h"

#define RANK_COORDINATOR 0

int Coordinator::m_emailsSent      = 0;
int Coordinator::m_totalEmails     = 0;
int Coordinator::m_repliesReceived = 0;
int Coordinator::m_activeWorkers   = 0;

EmailReader Coordinator::reader;
EmailWriter Coordinator::writer;

Coordinator::Coordinator() {
}

void Coordinator::mainLoop(const char* emailSource, const char* emailDest, bool serialized) {
	
	// Load emails from the source file
	// Send the OK if the file checks out
	int OK;
	
	try {
		printf("Loading XML data\n");
		reader.loadFromFile(emailSource);
		writer.useFile(emailDest);
		
		m_totalEmails = reader.getEmailCount();
		
		for (unsigned int i = 0; i < m_totalEmails; i++) {
			reader.get(i);
		}
		
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
	std::thread worker;
	
	if (!serialized) {
		worker = std::thread(receiveResult);
	}
	
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
		
		if (serialized) {
			talkWithNode(status.Get_source());
		} else {
			std::thread(talkWithNode, status.Get_source()).detach();
			
			// Sleep. We do not want this loop to be too tight.
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}
	
	// Wait for the worker to finish
	printf("Email distribution finished, waiting on results...\n");
	
	if (serialized) {
		receiveResult();
	} else {
		worker.join();
	}
	
	return;
}

void Coordinator::talkWithNode(int nodeId) {
	
	// Receive quantity of emails to send
	unsigned int quantity;
	MPI::COMM_WORLD.Recv(&quantity, 1, MPI::UNSIGNED, nodeId, TAG_EMAILS_REQUEST);
	
	printf("Received request from node #%i for %i emails\n", nodeId, quantity);
	
	// Calculate how many emails are left, and then use the smaller quantity
	unsigned int emailsRemaining = m_totalEmails - m_emailsSent;
	unsigned int sendingQuantity;
	
	if (emailsRemaining < quantity) {
		sendingQuantity = emailsRemaining;
	} else {
		sendingQuantity = quantity;
	}
	
	// Otherwise we send them all
	printf("Will send %i emails to node #%i\n", sendingQuantity, nodeId);
		
	// Set this now so other threads will not compete with this one
	int offset = m_emailsSent;
	m_emailsSent += sendingQuantity;
	
	// Tell the worker node how many emails to expect
	MPI::COMM_WORLD.Send(&sendingQuantity, 1, MPI::UNSIGNED, nodeId, TAG_EMAIL_QUANTITY);
	
	// If there are no more emails to send, exit
	if (sendingQuantity == 0) {
		printf("Sent termination signal to node #%i\n", nodeId);
		m_activeWorkers--;
		return;
	}	
	
	printf("Sending emails...\n");
	
	// The worker node is entitled to emails number (emailsSent) till (emailSent + sendingQuantity)
	// No other nodes should be fighting the node over this
	for (unsigned int emailId = offset; emailId < offset + sendingQuantity; emailId++) {
		
		Email e = reader.get(emailId);
		
		MPI::COMM_WORLD.Send(&emailId, 1, MPI::UNSIGNED, nodeId, TAG_EMAIL_ID);
		
		MPI_Send_string(e.Sender,  nodeId, TAG_EMAIL_SENDER);
		MPI_Send_string(e.Subject, nodeId, TAG_EMAIL_SUBJECT);
		MPI_Send_string(e.Body,    nodeId, TAG_EMAIL_BODY);
		MPI_Send_bool(e.IsValid,   nodeId, TAG_EMAIL_VALID);
		
		MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::UNSIGNED, nodeId, TAG_EMAIL_NUM_ATTACHMENTS);
		for (unsigned int j = 0; j < e.NumAttachments; j++) {
			MPI_Send_string(e.Attachments[j], nodeId, TAG_EMAIL_ATTACHMENT);
		}
		
		printf("Successfully sent email #%i to node #%i\n", emailId, nodeId);
	}
	
	printf("Successfully sent all %i emails to node #%i\n", sendingQuantity, nodeId);
}

void Coordinator::receiveResult() {
	
	while (m_repliesReceived < m_totalEmails) {
		
		MPI::Status status;
		int nonce;
		
		// Receive the nonce (first data point) from any source, then follow up with the rest from the same source only
		MPI::COMM_WORLD.Recv(&nonce, 1, MPI::UNSIGNED, MPI::ANY_SOURCE, TAG_RETURN_NONCE, status);
		
		int nodeId = status.Get_source();
		
		unsigned int emailId;
		MPI::COMM_WORLD.Recv(&emailId, 1, MPI::UNSIGNED, nodeId, TAG_RETURN_EMAIL_ID + nonce);
		
		Email result = reader.get(emailId);
		MPI::COMM_WORLD.Recv(&(result.SpamPercentage), 1, MPI::FLOAT, nodeId, TAG_RETURN_SPAM_PERCENTAGE + nonce);
		
		printf("Received spam percentage for email #%i from node #%i\n", emailId, nodeId);
		writer.add(result);
		
		m_repliesReceived++;
	}
}