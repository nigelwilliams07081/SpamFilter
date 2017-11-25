#!/bin/bash
mpiCC -o SpamFilter_exe Main.cpp Coordinator.cpp Worker.cpp EmailReader.cpp -std=c++11 -lstdc++