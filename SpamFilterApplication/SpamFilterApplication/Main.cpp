#include "stdafx.h"

#define RANK_COORDINATOR 0

int main(int argc, char **argv) {
	
	int prov = 0;
	
	#ifdef SINGLETHREADED
	MPI::Init();
	#else
	HPMPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &prov);
	#endif
	
	int taskId = MPI::COMM_WORLD.Get_rank();
	
	if (argc < 3) {
		if (taskId == RANK_COORDINATOR)
			printf("Must specify an absolute path to the input and output XML file!\n");
		
		MPI::Finalize();
		return 1;
	}
	
	int nodeCount = MPI::COMM_WORLD.Get_size();
	if (nodeCount == 1) {
		if (taskId == RANK_COORDINATOR)
			printf("Number of tasks must be more than 1!\n");
		
		MPI::Finalize();
		return 2;
	}
	
	if (taskId == RANK_COORDINATOR) {
		printf("Spam Filter MPI program version 0.9.02\n");
		#ifdef SINGLETHREADED
		printf("This program compiled in single-threaded mode.\n");
		#else
		printf("This program compiled in fully threaded mode.\n");
		#endif
	}
	
	#ifndef SINGLETHREADED
	if (prov < MPI::THREAD_MULTIPLE) {
		if (taskId == RANK_COORDINATOR)
			printf("Insufficient threading support provided! MPI_Init_thread returned %i\n", prov);
		
		MPI::Finalize();
		return 3;
	}
	#endif
	
	if (taskId == RANK_COORDINATOR) {
		
		const char *emailSource = argv[1];
		const char *emailDest   = argv[2];
		
		printf("Will load emails from %s and place output in %s\n", emailSource, emailDest);
		Coordinator::mainLoop(emailSource, emailDest);
		printf("Coordinator finished\n");
	} else {
		Worker::mainLoop();
		printf("Worker finished\n");
	}
	
	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();
	return 0;
}