#pragma once
#include "stdafx.h"

inline void MPI_Send_string(std::string s, int dest, int tag) {
	int len = s.size();
	
	char* sc = new char[len + 1];
	std::copy(s.begin(), s.end(), sc);
	sc[len] = '\0'; // Need this null terminator
	
	MPI::COMM_WORLD.Send(sc, len + 1, MPI::CHAR, dest, tag);
}

inline std::string MPI_Recv_string(int source, int tag, MPI::Status &status) {
	MPI::Status s;	
	MPI::COMM_WORLD.Probe(source, tag, s);
	int length = s.Get_count(MPI::CHAR);
	
	char* buffer = new char[length];
	MPI::COMM_WORLD.Recv(buffer, length, MPI::CHAR, source, tag, status);
	std::string result(buffer);
	delete[] buffer;
	return result;
}

inline std::string MPI_Recv_string(int source, int tag) {
	MPI::Status s;	
	MPI::COMM_WORLD.Probe(source, tag, s);
	int length = s.Get_count(MPI::CHAR);
	
	char* buffer = new char[length];
	MPI::COMM_WORLD.Recv(buffer, length, MPI::CHAR, source, tag);
	std::string result(buffer);
	delete[] buffer;
	return result;
}
