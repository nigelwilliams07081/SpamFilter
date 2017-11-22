#!/bin/bash
mpiCC -o SpamFilter main.cpp Coordinator.cpp Worker.cpp EmailReader.cpp -pthread -Wno-deprecated -std=c++11 -lstdc++
