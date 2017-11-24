#include "stdafx.h"

#define RANK_COORDINATOR 0
#define REQUEST_EMAILS_COUNT 10

Worker::Worker() {
}

void Worker::mainLoop() {
	printf("Worker started\n");
	// Wait for the OK broadcast from the coordinator
	int OK;
	MPI::COMM_WORLD.Bcast(&OK, 1, MPI::INT, RANK_COORDINATOR);
	
	// If something is wrong, just exit right away
	if (!OK) {
		return;
	}
	
	while(true) {
		
		// Send how many emails we would like
		int emailsCount = REQUEST_EMAILS_COUNT;
		MPI::COMM_WORLD.Send(&emailsCount, 1, MPI::INT, RANK_COORDINATOR, TAG_EMAILS_REQUEST);
		
		// The coordinator tells us how many emails to expect (may be less than how many we asked for)
		int quantity;
		MPI::COMM_WORLD.Recv(&quantity, 1, MPI::INT, RANK_COORDINATOR, TAG_EMAIL_QUANTITY);
				
		// If the coordinator is out of emails, we can exit
		if (quantity == 0) {
			return;
		}
		
		
		// Otherwise, we loop and recieve the emails
		for (unsigned int i = 0; i < quantity; i++) {
			Email e;
			
			e.Sender  = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_SENDER);
			e.Subject = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_SUBJECT);
			e.Body    = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_BODY);
			
			e.NumAttachments;
			MPI::COMM_WORLD.Recv(&(e.NumAttachments), 1, MPI::INT, RANK_COORDINATOR, TAG_EMAIL_NUM_ATTACHMENTS);
			
			e.Attachments = new std::string[e.NumAttachments];
			for (int i = 0; i < e.NumAttachments; i++) {
				e.Attachments[i] = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_ATTACHMENT);
			}
			
			// Spawn a new thread to process the email
			//std::thread(processEmail, e).detach();
			processEmail(e);
		}
	}
}

void Worker::processEmail(Email e) {
	// Process the email here
	//e.SpamPercentage = doSpamSearch(e);
	printf("Worker processing emails!\n");
	// Pass back to coordinator when we're done
	MPI_Send_string(e.Sender,  RANK_COORDINATOR, TAG_RETURN_EMAIL_SENDER);
	MPI_Send_string(e.Subject, RANK_COORDINATOR, TAG_RETURN_EMAIL_SUBJECT);
	MPI_Send_string(e.Body,    RANK_COORDINATOR, TAG_RETURN_EMAIL_BODY);
	
	MPI::COMM_WORLD.Send(&(e.SpamPercentage), 1, MPI::FLOAT, RANK_COORDINATOR, TAG_RETURN_EMAIL_SPAM_PERCENTAGE);
	MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::INT,   RANK_COORDINATOR, TAG_RETURN_EMAIL_NUM_ATTACHMENTS);
	
	for (int i = 0; i < e.NumAttachments; i++) {
		MPI_Send_string(e.Attachments[i], RANK_COORDINATOR, TAG_RETURN_EMAIL_ATTACHMENT);
	}
}