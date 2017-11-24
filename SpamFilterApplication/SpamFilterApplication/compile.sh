#!/bin/bash
mpiCC -o SpamFilter_exe Main.cpp Coordinator.cpp Worker.cpp EmailReader.cpp -std=c++11 -lstdc++

# For bsub
#bsub -n 4 -R "span[ptile=1]" -o %J.txt  "/opt/ibm/platform_mpi/bin/mpirun -lsf -vapi /home/nigelwilliams0708/Parallel-and-Distributed/T estMPISpamFilter/SpamFilter_exe /home/nigelwilliams0708/Parallel-and-Distributed/TestMPISpamFilt er/testXML.xml"

# Flexible version
bsub -n 4 -R "span[ptile=1]" -o %J.txt  "/opt/ibm/platform_mpi/bin/mpirun -lsf -vapi $PWD/SpamFilter_exe $PWD/testXML.xml"