#include "stdafx.h"

#include "Coordinator.h"
#include "Worker.h"

#define RANK_COORDINATOR 0

int main(int argc, char **argv) {

	if (argc == 0) {
		print("Must specify an absolute path to the email XML file!\n");
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
	
	const char *emailSource = argv[0];
	printf("Will load emails from %s\n", emailSource);
	
	if (taskId == RANK_COORDINATOR) {
		Coordinator::mainLoop(emailSource);
	} else {
		Worker::mainLoop();
	}
	
	MPI_Finalize();
	return 0;
}