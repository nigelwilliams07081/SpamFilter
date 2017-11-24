#pragma once
#include <mpiCC.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>

#include "Bayesian.h"
#include "SpamFilter.h"
//#include "EmailReceiver.h"
#include "EmailReader.h"
#include "FileNames.h"
#include "Email.h"
#include "Worker.h"
#include "Coordinator.h"
#include "tags.h"
#include "MPI_wrappers.h"

#define SINGLETHREADED 1