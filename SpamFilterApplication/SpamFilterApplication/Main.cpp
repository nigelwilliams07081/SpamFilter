#include "stdafx.h"

//int main()
//{
//	EmailReader emailReader;
//
//	emailReader.loadFromFile("emails.xml");
//
//	emailReader.begin();
//	Email email = emailReader.next();
//
//	printf("Address:%s\n\nSubject:\n%s\n\nBody:\n%s\n\nAttachments:\n%s\n", email.Sender, email.Subject, email.Body, email.Attachments);
//
//	/*SpamFilter spamFilter;
//	Bayesian bayesianCalculator;
//
//	spamFilter.PerformSpamSearch();
//
//	bayesianCalculator.SetAnyMsgIsSpam(0.8f);
//	bayesianCalculator.SetWordAppearsInSpam(spamFilter.GetOverallSpamConfidence());
//	bayesianCalculator.CalculateTheorem();
//
//	std::cout << "Our spam confidence is: " << spamFilter.GetOverallSpamConfidence() << " -> " << spamFilter.GetOverallSpamConfidence() * 100.0f << "%" << std::endl;
//	std::cout << "The Bayesian confidence is: " << bayesianCalculator.GetGivenMsgIsSpam() << " -> " << bayesianCalculator.GetGivenMsgIsSpam() * 100.0f << "%" << std::endl;
//	*/
//
//	return 0;
//}

#define RANK_COORDINATOR 0

int main(int argc, char **argv) {
	
	int prov = MPI::Init_thread(MPI::THREAD_MULTIPLE);
	
	if (argc == 1) {
		printf("Must specify an absolute path to the email XML file!\n");
		MPI::Finalize();
		return 1;
	}
	
	int nodeCount = MPI::COMM_WORLD.Get_size();

	if (nodeCount == 1) {
		printf("Number of tasks must be more than 1!\n");
		MPI::Finalize();
		return 2;
	}

	int taskId = MPI::COMM_WORLD.Get_rank();
	
	char finished;
	
	if (taskId == RANK_COORDINATOR) {
		
		printf("single: %i\nfunneled: %i\nserialized: %i\nmultiple: %i\n", MPI::THREAD_SINGLE, MPI::THREAD_FUNNELED, MPI::THREAD_SERIALIZED, MPI::THREAD_MULTIPLE);
		printf("Thread support provided: %i\n", prov);
		
		const char *emailSource = argv[1];
		printf("Will load emails from %s\n", emailSource);
		Coordinator::mainLoop(emailSource);
		
		// Finish
		printf("Coordinator finished\n");
		MPI::COMM_WORLD.Barrier();
	} else {
		Worker::mainLoop();
		
		// Wait here for the coordinator to receive all the data
		printf("Worker finished\n");
		MPI::COMM_WORLD.Barrier();
	}
	
	
	MPI::Finalize();
	return 0;
}