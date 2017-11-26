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
		
		// Sleep. We do not want this loop to be too tight.
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		#endif
	}
	
	// Wait for the worker to finish
	printf("Email distribution finished, waiting on results...\n");
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
	
	// Otherwise we send them all
	printf("Will send %i emails to node #%i\n", sendingQuantity, nodeId);
		
	// Set this now so other threads will not compete with this one
	m_emailsSent += sendingQuantity;
	
	// Tell the worker node how many emails to expect
	MPI::COMM_WORLD.Send(&sendingQuantity, 1, MPI::INT, nodeId, TAG_EMAIL_QUANTITY);
	
	// If there are no more emails to send, exit
	if (sendingQuantity == 0) {
		printf("Sent termination signal to node #%i\n", nodeId);
		m_activeWorkers--;
		return;
	}	
	
	printf("Sending emails...\n");
	
	for (int i = 0; i < sendingQuantity; i++) {
		// The worker node is entitled to emails number (emailsSent) till (emailSent + sendingQuantity)
		// No other nodes should be fighting the node over this
		Email e = reader.get(m_emailsSent - i - 1);
		
		MPI_Send_string(e.Sender,  nodeId, TAG_EMAIL_SENDER);
		MPI_Send_string(e.Subject, nodeId, TAG_EMAIL_SUBJECT);
		MPI_Send_string(e.Body,    nodeId, TAG_EMAIL_BODY);
		
		MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::INT, nodeId, TAG_EMAIL_NUM_ATTACHMENTS);
		int isValid = (int)(e.IsValid);
		MPI::COMM_WORLD.Send(&isValid, 1, MPI::INT, nodeId, TAG_EMAIL_VALID);
		
		for (unsigned int j = 0; j < e.NumAttachments; j++) {
			MPI_Send_string(e.Attachments[j], nodeId, TAG_EMAIL_ATTACHMENT);
		}
		
		printf("Successfully sent email to node #%i\n", nodeId);
	}
	
	printf("Successfully sent all %i emails to node #%i\n", sendingQuantity, nodeId);
}

void Coordinator::receiveResult() {
	
	while (m_repliesReceived < m_totalEmails) {
		
		Email result;
		MPI::Status status;
		int nonce;
		
		// Receive the nonce (first data point) from any source, then follow up with the rest from the same source only
		MPI::COMM_WORLD.Recv(&nonce, 1, MPI::UNSIGNED, MPI::ANY_SOURCE, TAG_RETURN_EMAIL_NONCE, status);
		
		int node = status.Get_source();
		
		printf("Received email data nonce %i from node #%i\n", nonce, node);
		
		result.Sender  = MPI_Recv_string(node, TAG_RETURN_EMAIL_SENDER  + nonce);
		result.Subject = MPI_Recv_string(node, TAG_RETURN_EMAIL_SUBJECT + nonce);
		result.Body    = MPI_Recv_string(node, TAG_RETURN_EMAIL_BODY    + nonce);
		
		float spamPercent;
		MPI::COMM_WORLD.Recv(&(result.NumAttachments), 1, MPI::UNSIGNED, node, TAG_RETURN_EMAIL_NUM_ATTACHMENTS + nonce);

		int isValid = 0;
		MPI::COMM_WORLD.Recv(&isValid, 1, MPI::INT, node, TAG_RETURN_EMAIL_NUM_ATTACHMENTS + nonce);
		result.IsValid = isValid;

		MPI::COMM_WORLD.Recv(&spamPercent,             1, MPI::FLOAT,    node, TAG_RETURN_EMAIL_SPAM_PERCENTAGE + nonce);
		result.SpamPercentage = spamPercent;
		
		if (result.NumAttachments != 0) {
			result.Attachments = new std::string[result.NumAttachments];
			for (int i = 0; i < result.NumAttachments; i++) {
				result.Attachments[i] = MPI_Recv_string(node, TAG_RETURN_EMAIL_ATTACHMENT + nonce);
			}
		}
		
		printf("Analyzed email #%i recieved from node #%i! Subject was %s\n", m_repliesReceived, node, result.Subject.c_str());
		m_repliesReceived++;
		
		// Result now contains an email that has been analyzed for spam
		// Do something with it here
		//EmailWriter.add(result);
	}
	
	//EmailWriter.writeToFile("output.xml");
	
}