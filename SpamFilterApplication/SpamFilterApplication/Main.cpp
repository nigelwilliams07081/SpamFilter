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

#define RANK_COORDINATOR 0

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Must specify an absolute path to the email XML file!\n");
		return 1;
	}
	
	MPI_Init(&argc, &argv);
	
	int nodeCount;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nodeCount);

	if (nodeCount == 1) {
		printf("Number of tasks must be more than 1!\n");
		MPI_Finalize();
		return 2;
	}

	int taskId;
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	
	if (taskId == RANK_COORDINATOR) {
		const char *emailSource = argv[1];
		printf("Will load emails from %s\n", emailSource);
		Coordinator::mainLoop(emailSource);
	} else {
		Worker::mainLoop();
	}
	
	MPI_Finalize();
	return 0;
}