#include "stdafx.h"

#include <thread>

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
		reader.loadFromFile(emailsource);
		
		// Make sure every email does not cause any parse errors, also count them all
		while (reader.hasNext()) {
			Email t = reader.next();
			m_totalEmails++;
		}
		
		// Reset reader afterwards
		reader.begin();
		
		// Everything checks out
		OK = (int)true;
	} catch (const std::exception &e) {
		// Some exception from the XML parser came up
		// Perhaps the XML was malformed, the file could not be opened at all, etc
		OK = (int)false;
	}
	
	// Broadcast the OK to all nodes
	MPI_Bcast(&OK, 1, MPI_INT, RANK_COORDINATOR, MPI_COMM_WORLD);
	
	if (!OK) {
		printf("Error reading XML data. Exiting\n");
		//MPI_Finalize();
		return;
	}
		
	// Start up a thread to receive spam anaylsis results
	std::thread worker(receiveResult);
	
	// Wait for threads to ask for a quantity of emails
	while (reader.hasNext()) {
		
		MPI_Status status;
		
		char dummy;
		MPI_Recv(&dummy, 1, MPI_CHAR, MPI_ANY_SOURCE, TAG_EMAILS_REQUEST, MPI_COMM_WORLD, &status);
		
		// We recieved a request for emails, spawn a new thread to serve it
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
	MPI_Recv(&quantity, 1, MPI_INT, nodeId, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
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
	
	int repliesReceived = 0;
	
	while (!m_finished && repliesReceived < m_emailsSent) {
		
		Email result;
		MPI_Recv(&result, sizeof(Email), MPI_BYTE, MPI_ANY_SOURCE, TAG_EMAIL_ANALYZED, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		repliesReceived++;
		
		// Result now contains an email that has been analyzed for spam
		// Do something with it here
	}
	
	return;
}