#include "stdafx.h"

#define Error(...) if (isCoordinator) { printf("ERROR: " __VA_ARGS__); } MPI::Finalize(); return 1;
#define Log(...) if (isCoordinator) { printf(__VA_ARGS__); }

#define RANK_COORDINATOR 0
#define THREAD_UNDEFINED -1
#define THREAD_DEFAULT 5

int main(int argc, char **argv) {
	
	int prov = 0;
	HPMPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &prov);
	
	int taskId = MPI::COMM_WORLD.Get_rank();
	bool isCoordinator = taskId == RANK_COORDINATOR;
	
	int nodeCount = MPI::COMM_WORLD.Get_size();
	if (nodeCount == 1) {
		Error("Number of tasks must be more than 1\n");
	}
	
	// Argument variables
	const char *emailSource = NULL;
	const char *emailDest   = NULL;
	bool serialized         = false;
	int threads             = THREAD_UNDEFINED;
	
	
	// Begin parsing command line arguments
	for (unsigned int j = 1; j != argc; j++) {
			
		// Handle input file
		if (argv[j] == std::string("-i") || argv[j] == std::string("-inputfile")) {
			if (++j == argc) {
				Error("No input file provided\n");
			}
			
			emailSource = argv[j];
			Log("Will load input from %s\n", emailSource);
		
		
		
		// Handle output file
		} else if (argv[j] == std::string("-o") || argv[j] == std::string("-outputfile")) {
			if (++j == argc) {
				Error("No output file provided\n");
			}
			
			emailDest = argv[j];
			Log("Will save output to %s\n", emailDest);
		
		
		
		// Handle thread count
		} else if (argv[j] == std::string("-t") || argv[j] == std::string("-threads")) {
			if (++j == argc) {
				Error("No thread count provided\n");
			}
			
			if (serialized) {
				Error("Cannot have threads while serialized\n");
			}
			
			try {
				threads = std::stoi(argv[j]);
			} catch (const std::invalid_argument &e) {
				Error("Thread count is not an integer\n");
			} catch (const std::out_of_range &e) {
				Error("Thread count is outside integer range\n");
			}
			
			if (threads <= 0) {
				Error("Thread count is zero or negative\n");
			}
			
			Log("Will use %i worker threads\n", threads);
		
		
		
		// Handle serialized runtime
		} else if (argv[j] == std::string("--serialized")) {
			
			// In order to be serialized we must have received -t 1 or the -t flag must have not be present
			if (threads != 1 && threads != THREAD_UNDEFINED) {
				Error("Cannot have threads while serialized\n");
			}
			
			serialized = true;
			threads = 1;
			Log("Will run in serialized mode\n");
		
		
		
		// Handle invalid argument	
		} else {
			Error("Invalid argument: %s\n", argv[j]);
		}
	}
	
	// Post-parsing checks
	
	// Ensure the serializing + threading settings aren't limited by MPI
	if (!serialized) {
		if (prov < MPI::THREAD_MULTIPLE) {
			Error("Insufficient threading support provided: MPI_Init_thread returned %i. Are you linking mtdlib?\n", prov);
		}
	}
	
	// Ensure input and output files were provided
	if (emailSource == NULL) {
		Error("No input file provided (-i <file>)\n");
	}
	
	if (emailDest == NULL) {
		Error("No output file provided (-o <file>)\n");
	}
	
	// Notify if we didn't get any thread flag and use default threads
	if (threads == THREAD_UNDEFINED) {
		Log("No thread count provided (-t <integer>), defaulting to %i\n", THREAD_DEFAULT);
		threads = THREAD_DEFAULT;
	}
	
	
	// Everything has checked out - attempt to run the main node-specific code
	Log("Spam Filter MPI program version 0.9.05\n");
	
	if (isCoordinator) {
		Coordinator::mainLoop(emailSource, emailDest, serialized);
		printf("Coordinator finished\n");
	} else {
		Worker::mainLoop(threads);
		printf("Worker finished\n");
	}
	
	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();
	return 0;
}