#!/bin/bash
mpiCC -o SpamFilter Main.cpp Coordinator.cpp Worker.cpp EmailReader.cpp EmailWriter.cpp StaticSpamFilter.cpp -std=c++11 -lstdc++