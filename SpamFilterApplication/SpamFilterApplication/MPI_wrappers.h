#pragma once
#include "stdafx.h"

int MPI_Send_string(std::string s, int dest, int tag, MPI_Comm comm) {
	return MPI_Send(s.c_str(), s.size(), MPI_CHAR, dest, tag, comm);
}

std::string MPI_Recv_string(int source, int tag, MPI_Comm comm, MPI_Status *status) {
	MPI_Status s;
	int length;
	
	MPI_Probe(source, tag, comm, &s);
	MPI_Get_count(&s, MPI_CHAR, &length);
	
	char* buffer = new char[length];
	MPI_Recv(&buffer, length, MPI_CHAR, source, tag, comm, status);
	std::string result = buffer;
	delete[] buffer;
	return result;
}
	