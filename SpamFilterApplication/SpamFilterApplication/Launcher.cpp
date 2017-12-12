#include "stdafx.h"

#define Error(...) if (isCoordinator) { printf("ERROR: " __VA_ARGS__); } MPI::Finalize(); return 1;
#define Log(...) if (isCoordinator) { printf(__VA_ARGS__); }

#define RANK_COORDINATOR 0
#define THREAD_UNDEFINED -1
#define THREAD_DEFAULT 5
#define TIME_UNLIMITED -1

void exitAfter(int seconds) {
	std::this_thread::sleep_for(std::chrono::seconds(seconds));
	TimeCout << "Time limit expired, aborting...\n";
	MPI::COMM_WORLD.Abort(1);
	exit(1);
}

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
	int timelimit           = TIME_UNLIMITED;
	
	
	// Begin parsing command line arguments
	for (unsigned int j = 1; j != argc; j++) {
		
		std::string arg(argv[j]);
		bool lastArg = j + 1 == argc;
		
		
		// Handle input file
		//---------------------------
		if (
			arg == "-i" ||
		    arg == "-inputfile"
		) {
			
			if (lastArg) {
				Error("No input file provided\n");
			}
			
			emailSource = argv[++j];
			Log("Will load input from %s\n", emailSource);
		}
		//---------------------------
		
		
		
		// Handle output file
		//---------------------------
		else if (
			arg == "-o" || 
			arg == "-outputfile"
		) {
			
			if (lastArg) {
				Error("No output file provided\n");
			}
			
			emailDest = argv[++j];
			Log("Will save output to %s\n", emailDest);
		}
		//---------------------------
		
		
		
		// Handle thread count
		//---------------------------
		else if (
			arg == "-t" || 
			arg == "-threads"
		) {
			
			if (lastArg) {
				Error("No thread count provided\n");
			}
			
			if (serialized) {
				Error("Cannot have threads while serialized\n");
			}
			
			try {
				threads = std::stoi(argv[++j]);
			} catch (const std::invalid_argument &e) {
				Error("Thread count is not an integer\n");
			} catch (const std::out_of_range &e) {
				Error("Thread count is outside integer range\n");
			}
			
			if (threads <= 0) {
				Error("Thread count is zero or negative\n");
			}
			
			Log("Will use %i worker threads\n", threads);
		}
		//---------------------------
		
		
		
		// Handle max execution time
		//---------------------------
		else if (
			arg == "-m" || 
			arg == "-maxtime"
		) {
			
			if (lastArg) {
				Error("No time limit provided\n");
			}
			
			if (serialized) {
				Error("Cannot time program while serialized\n");
			}
			
			try {
				timelimit = std::stoi(argv[++j]);
			} catch (const std::invalid_argument &e) {
				Error("Time limit is not an integer\n");
			} catch (const std::out_of_range &e) {
				Error("Time limit is outside integer range\n");
			}
			
			if (timelimit <= 0) {
				Error("Time limit is zero or negative\n");
			}
			
			Log("Will exit after %i seconds\n", timelimit);
		}
		//---------------------------
		
		
		
		// Handle serialized runtime
		//---------------------------
		else if (arg == "--serialized") {
			
			// In order to be serialized we must have received -t 1 or the -t flag must have not be present
			if (threads != 1 && threads != THREAD_UNDEFINED) {
				Error("Cannot have threads while serialized\n");
			}
			
			if (timelimit != TIME_UNLIMITED) {
				Error("Cannot time program while serialized\n");
			}
			
			serialized = true;
			threads = 1;
			Log("Will run in serialized mode\n");
		}
		//---------------------------
		
		
		
		// Handle invalid argument
		//---------------------------
		else {
			Error("Invalid argument: %s\n", argv[j]);
		}
		//---------------------------
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
	
	// Set time limit if we are using one
	if (timelimit != TIME_UNLIMITED) {
		std::thread(exitAfter, timelimit).detach();
	}
	
	
	// Everything has checked out - attempt to run the main node-specific code
	Log("Spam Filter MPI program version 0.9.10\n");
	
	MPI::COMM_WORLD.Barrier();
	
	if (isCoordinator) {
		Coordinator::mainLoop(emailSource, emailDest, serialized);
		TimeCout << "Coordinator finished\n";
	} else {
		Worker::mainLoop(threads);
		TimeCout << "Worker finished\n";
	}
	
	MPI::COMM_WORLD.Barrier();
	
	MPI::Finalize();
	return 0;
}