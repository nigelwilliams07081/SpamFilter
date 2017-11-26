#include "stdafx.h"

#define RANK_COORDINATOR 0

int main(int argc, char **argv) {
	
	int prov = 0;
	HPMPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &prov);
	
	int nodeCount = MPI::COMM_WORLD.Get_size();
	if (nodeCount == 1) {
		printf("Error: number of tasks must be more than 1\n");
		MPI::Finalize();
		return 1;
	}
	
	int taskId = MPI::COMM_WORLD.Get_rank();
	bool isCoordinator = taskId == RANK_COORDINATOR;
	
	// Argument variables
	const char *emailSource = NULL;
	const char *emailDest   = NULL;
	bool serialized         = false;
	int threads             = 1;
	
	
	// Begin parsing command line arguments
	for (unsigned int j = 1; j != argc; j++) {
			
		// Handle input file
		if (argv[j] == std::string("-i") || argv[j] == std::string("-inputfile")) {
			j++;
			if (j == argc) {
				if (isCoordinator) printf("Error: no input file provided\n");
				MPI::Finalize();
				return 2;
			}
			
			emailSource = argv[j];
			if (isCoordinator) printf("Will load input from %s\n", emailSource);
		
		// Handle output file
		} else if (argv[j] == std::string("-o") || argv[j] == std::string("-outputfile")) {
			j++;
			if (j == argc) {
				if (isCoordinator) printf("Error: no output file provided\n");
				MPI::Finalize();
				return 3;
			}
			
			emailDest = argv[j];
			if (isCoordinator) printf("Will save output to %s\n", emailDest);
		
		
		// Handle thread count
		} else if (argv[j] == std::string("-t") || argv[j] == std::string("-threads")) {
				
			j++;
			if (j == argc) {
				if (isCoordinator) printf("Error: no thread count provided\n");
				MPI::Finalize();
				return 4;
			}
			
			if (serialized) {
				if (isCoordinator) printf("Error: cannot have threads while serialized\n");
				MPI::Finalize();
				return 5;
			}
			
			try {
				threads = std::stoi(argv[j]);
			} catch (const std::invalid_argument &e) {
				if (isCoordinator) printf("Error: thread count is not an integer");
				MPI::Finalize();
				return 6;
			} catch (const std::out_of_range &e) {
				if (isCoordinator) printf("Error: thread count is outside integer range");
				MPI::Finalize();
				return 7; 
			}
			
			if (threads <= 0) {
				if (isCoordinator) printf("Error: thread count is zero or negative");
				MPI::Finalize();
				return 8; 
			}
			
			if (isCoordinator) printf("Will use %i worker threads\n", threads);
				
			
			
		// Handle serialized runtime
		} else if (argv[j] == std::string("--serialized")) {
			if (threads != 1) {
				if (isCoordinator) printf("Error: cannot have threads while serialized\n");
				MPI::Finalize();
				return 9;
			}
			
			serialized = true;
			if (isCoordinator) printf("Will run in serialized mode\n");
		
		
		// Handle invalid argument	
		} else {
			if (isCoordinator) printf("Error: invalid argument: %s\n", argv[j]);
			MPI::Finalize();
			return 10;
		}
	}
	
	// Post-parsing checks
	
	// Ensure the serializing + threading settings aren't limited by MPI
	if (!serialized) {
		if (prov < MPI::THREAD_MULTIPLE) {
			if (isCoordinator) printf("Insufficient threading support provided: MPI_Init_thread returned %i. Are you linking mtdlib?\n", prov);
			
			MPI::Finalize();
			return 11;
		}
	}
	
	// Ensure input and output files were provided
	if (emailSource == NULL) {
		if (isCoordinator) printf("Error: no input file provided\n");
		MPI::Finalize();
		return 12;
	}
	
	if (emailDest == NULL) {
		if (isCoordinator) printf("Error: no output file provided\n");
		MPI::Finalize();
		return 13;
	}
	
	if (isCoordinator) {
		printf("Spam Filter MPI program version 0.9.03\n");
		Coordinator::mainLoop(emailSource, emailDest, serialized);
		printf("Coordinator finished\n");
	} else {
		Worker::mainLoop(threads, serialized);
		printf("Worker finished\n");
	}
	
	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();
	return 0;
}