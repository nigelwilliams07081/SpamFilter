#pragma once
#ifndef _STDAFX_H
#define _STDAFX_H

#include <mpiCC.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cassert>
#include <functional>

#include "Bayesian.h"
#include "SpamFilter.h"
#include "EmailReader.h"
#include "EmailWriter.h"
#include "FileNames.h"
#include "Email.h"
#include "Worker.h"
#include "Coordinator.h"
#include "tags.h"
#include "MPI_wrappers.h"

#endif