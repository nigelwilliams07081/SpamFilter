#include "stdafx.h"

#include "tags.h"
#include "Worker.h"

#include <thread>

#define RANK_COORDINATOR 0
#define REQUEST_EMAILS_COUNT 10

Worker::Worker() {
}

void Worker::mainLoop() {
	
	// Wait for the OK broadcast from the coordinator
	int OK;
	MPI_Bcast(&OK, 1, MPI_INT, RANK_COORDINATOR, MPI_COMM_WORLD);
	
	// If something is wrong, just exit right away
	if (!OK) {
		return;
	}
	
	while(true) {
		
		// Send how many emails we would like
		int emailsCount = REQUEST_EMAILS_COUNT;
		MPI_Send(&emailsCount, 1, MPI_INT, RANK_COORDINATOR, TAG_EMAILS_REQUEST, MPI_COMM_WORLD);
		
		// The coordinator tells us how many emails to expect (may be less than how many we asked for)
		int quantity;
		MPI_Recv(&quantity, 1, MPI_INT, RANK_COORDINATOR, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// If the coordinator is out of emails, we can exit
		if (quantity == 0) {
			return;
		}
		
		// Otherwise, we loop and recieve the emails
		for (unsigned int i = 0; i < quantity; i++) {
			Email e;
			
			e.Sender  = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_SENDER,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			e.Subject = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_SUBJECT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			e.Body    = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_BODY,    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			e.NumAttachments;
			MPI_Recv(&(e.NumAttachments), 1, MPI_INT, RANK_COORDINATOR, TAG_EMAIL_NUM_ATTACHMENTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			e.Attachments = new std::string[e.NumAttachments];
			for (int i = 0; i < e.NumAttachments; i++) {
				e.Attachments[i] = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_ATTACHMENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			
			// Spawn a new thread to process the email
			std::thread(processEmail, e).detach();
		}
	}
}

void Worker::processEmail(Email e) {
	// Process the email here
	//e.SpamPercentage = doSpamSearch(e);
	
	// Pass back to coordinator when we're done
	MPI_Send_string(e.Sender,  nodeId, TAG_RETURN_EMAIL_SENDER,  MPI_COMM_WORLD);
	MPI_Send_string(e.Subject, nodeId, TAG_RETURN_EMAIL_SUBJECT, MPI_COMM_WORLD);
	MPI_Send_string(e.Body,    nodeId, TAG_RETURN_EMAIL_BODY,    MPI_COMM_WORLD);
	
	MPI_Send(&(e.SpamPercentage), 1, MPI_FLOAT, nodeId, TAG_RETURN_EMAIL_SPAM_PERCENTAGE, MPI_COMM_WORLD);
	MPI_Send(&(e.NumAttachments), 1, MPI_INT,   nodeId, TAG_RETURN_EMAIL_NUM_ATTACHMENTS, MPI_COMM_WORLD);
	
	for (int i = 0; i < e.NumAttachments; i++) {
		MPI_Send_string(e.Attachments[i], nodeId, TAG_RETURN_EMAIL_ATTACHMENT, MPI_COMM_WORLD);
	}
}