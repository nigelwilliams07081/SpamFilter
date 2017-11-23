#include "stdafx.h"

#define RANK_COORDINATOR 0

int Coordinator::m_emailsSent = 0;
int Coordinator::m_totalEmails = 0;
bool Coordinator::m_finished = false;

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
		
		// Make sure every email does not cause any parse errors, also count them all
		while (reader.hasNext()) {
			Email t = reader.next();
			m_totalEmails++;
		}
		
		// Reset reader afterwards
		reader.begin();
		
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
	int nodeCount;
	MPI_Comm_size(MPI_COMM_WORLD, &nodeCount);
	
	// Loop a a few extra times to make sure we instruct all nodes to finish
	int loopCount = nodeCount - 1 + m_totalEmails;
	
	// Wait for threads to ask for a quantity of emails
	while (m_repliesReceived < m_totalEmails + loopCount) {
		printf("Waiting for worker node...\n");
		
		MPI_Status status;
		char dummy;
		MPI_Recv(&dummy, sizeof(char), MPI_CHAR, MPI_ANY_SOURCE, TAG_EMAILS_REQUEST, MPI_COMM_WORLD, &status);
		
		// We recieved a request for emails, spawn a new thread to serve it
		printf("Received a data request from node #%i\n", status.MPI_SOURCE);
		std::thread(talkWithNode, status.MPI_SOURCE).detach();
	}
	
	// Send a semaphore to the worker to thread using this static variable
	m_finished = true;
	
	// Wait for the worker to finish
	worker.join();
	
	return;
}

void Coordinator::talkWithNode(int nodeId) {
	
	// Receive quantity of emails to send
	int quantity;
	MPI_Recv(&quantity, sizeof(int), MPI_INT, nodeId, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
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
	printf("Sending %i emails to node #%i\n", sendingQuantity, nodeId);
	MPI_Send(&sendingQuantity, sizeof(int), MPI_INT, nodeId, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD);
	
	// If there are no more emails to send, exit
	if (sendingQuantity == 0) {
		return;
	}	
	
	// Otherwise we send them all
	for (int i = 0; i < sendingQuantity; i++) {
		Email e = reader.next();
		MPI_Send(&e, sizeof(Email), MPI_BYTE, nodeId, TAG_EMAIL_DATA, MPI_COMM_WORLD);
		m_emailsSent++;
	}
	
	return;
}

void Coordinator::receiveResult() {
	
	
	while (!m_finished && m_repliesReceived < m_emailsSent) {
		
		Email result;
		MPI_Recv(&result, sizeof(Email), MPI_BYTE, MPI_ANY_SOURCE, TAG_EMAIL_ANALYZED, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		m_repliesReceived++;
		
		printf("Analyzed email recieved from node!\n");
		// Result now contains an email that has been analyzed for spam
		// Do something with it here
	}
	
	return;
}