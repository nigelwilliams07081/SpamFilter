#include "stdafx.h"

#define RANK_COORDINATOR 0
#define REQUEST_EMAILS_COUNT 10

unsigned int Worker::m_nonce = 0;
bool *Worker::m_workingThreads = NULL;

Worker::Worker() {
}

void Worker::mainLoop(int threads) {
	printf("Worker started\n");
	
	// Wait for the OK broadcast from the coordinator
	int OK;
	MPI::COMM_WORLD.Bcast(&OK, 1, MPI::INT, RANK_COORDINATOR);
	
	// If something is wrong, just exit right away
	if (!OK) {
		return;
	}
	
	m_workingThreads = new bool[threads];
	
	while (true) {
		
		int availableThreads = 0;
		for (unsigned int t = 0; t < threads; t++) {
			if (!m_workingThreads[t]) {
				availableThreads++;
			}
		}
			
		// Send how many emails we would like
		MPI::COMM_WORLD.Send(&availableThreads, 1, MPI::INT, RANK_COORDINATOR, TAG_EMAILS_REQUEST);
		
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
			e.IsValid = MPI_Recv_bool(RANK_COORDINATOR, TAG_EMAIL_VALID);
			
			MPI::COMM_WORLD.Recv(&(e.NumAttachments), 1, MPI::UNSIGNED, RANK_COORDINATOR, TAG_EMAIL_NUM_ATTACHMENTS);
			
			e.Attachments = new std::string[e.NumAttachments];
			for (unsigned int j = 0; j < e.NumAttachments; j++) {
				e.Attachments[j] = MPI_Recv_string(RANK_COORDINATOR, TAG_EMAIL_ATTACHMENT);
			}
			
			// 1 thread is the same as being serialized for the worker
			if (threads == 1) {
				processEmail(e, 0);
			} else {
				// Find an empty thread to assign this email to
				for (unsigned int t = 0; t < threads; t++) {
					if (!m_workingThreads[t]) {
						std::thread(processEmail, e, t).detach();
						break;
					}
				}
			}
		}
	}
}

void Worker::processEmail(Email e, int threadNum) {
	
	// Mark this thread as active
	m_workingThreads[threadNum] = true;
	
	// Remember MPI_Send takes time. Reserve the nonce for this thread immediately
	int reserved = m_nonce;
	m_nonce = (m_nonce + 1) % 2048;

	//SpamFilter spamFilter;
	//spamFilter.SetEmail(e);

	// Process the email here
	//spamFilter.PerformSpamSearch();
	//e.SpamPercentage = spamFilter.GetOverallSpamConfidence();
	e.SpamPercentage = SSpamFilter::PerformSpamSearch(e);
	
	// Pass back to coordinator when we're done
	
	// The nonce prevents the coordinator from getting confused when a single node
	// is sending it 10 emails at once. Just add the nonce to the tags, and use that!
	// Came up with that at 7am after not sleeping.
	MPI::COMM_WORLD.Send(&reserved, 1, MPI::UNSIGNED, RANK_COORDINATOR, TAG_RETURN_EMAIL_NONCE);
	
	MPI_Send_string(e.Sender,  RANK_COORDINATOR, TAG_RETURN_EMAIL_SENDER  + reserved);
	MPI_Send_string(e.Subject, RANK_COORDINATOR, TAG_RETURN_EMAIL_SUBJECT + reserved);
	MPI_Send_string(e.Body,    RANK_COORDINATOR, TAG_RETURN_EMAIL_BODY    + reserved);
	
	MPI::COMM_WORLD.Send(&(e.SpamPercentage), 1, MPI::FLOAT,    RANK_COORDINATOR, TAG_RETURN_EMAIL_SPAM_PERCENTAGE + reserved);
	MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::UNSIGNED, RANK_COORDINATOR, TAG_RETURN_EMAIL_NUM_ATTACHMENTS + reserved);
	
	for (unsigned int i = 0; i < e.NumAttachments; i++) {
		MPI_Send_string(e.Attachments[i], RANK_COORDINATOR, TAG_RETURN_EMAIL_ATTACHMENT + reserved);
	}
	
	// Thread is finished, mark as inactive so it may be replaced
	m_workingThreads[threadNum] = false;
}