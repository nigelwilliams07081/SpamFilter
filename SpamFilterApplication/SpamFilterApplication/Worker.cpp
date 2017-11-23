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
	MPI_Bcast(&OK, sizeof(int), MPI_INT, RANK_COORDINATOR, MPI_COMM_WORLD);
	
	// If something is wrong, just exit right away
	if (!OK) {
		return;
	}
	
	while(true) {
		
		// Send request that we would like more emails
		char dummy = '\0';
		MPI_Send(&dummy, sizeof(char), MPI_CHAR, RANK_COORDINATOR, TAG_EMAILS_REQUEST, MPI_COMM_WORLD);
		
		// Send how many emails we would like
		int emailsCount = REQUEST_EMAILS_COUNT;
		MPI_Send(&emailsCount, sizeof(int), MPI_INT, RANK_COORDINATOR, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD);
		
		// The coordinator tells us how many emails to expect (may be less than how many we asked for)
		int quantity;
		MPI_Recv(&quantity, sizeof(int), MPI_INT, RANK_COORDINATOR, TAG_EMAIL_QUANTITY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// If the coordinator is out of emails, we can exit
		if (quantity == 0) {
			return;
		}
		
		// Otherwise, we loop and recieve the emails
		for (unsigned int i = 0; i < quantity; i++) {
			Email e;
			MPI_Recv(&e, sizeof(Email), MPI_BYTE, RANK_COORDINATOR, TAG_EMAIL_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// Spawn a new thread to process the email
			std::thread(processEmail, e).detach();
		}
	}
}

void Worker::processEmail(Email e) {
	// Process the email here
	//*e.SpamPercentage = doSpamSearch(*e);
	
	// Pass back to coordinator when we're done
	MPI_Send(e, sizeof(Email), MPI_BYTE, RANK_COORDINATOR, TAG_EMAIL_ANALYZED, MPI_COMM_WORLD);
}