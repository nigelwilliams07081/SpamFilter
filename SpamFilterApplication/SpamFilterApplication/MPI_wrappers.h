#pragma once
#include "stdafx.h"

inline void MPI_Send_string(std::string s, int dest, int tag) {
	int len = s.size();
	
	char* sc = new char[len + 1];
	std::copy(s.begin(), s.end(), sc);
	sc[len] = '\0'; // Need this null terminator
	
	MPI::COMM_WORLD.Send(sc, len + 1, MPI::UNSIGNED_CHAR, dest, tag);
	delete[] sc;
}

inline void MPI_Send_bool(bool b, int dest, int tag) {
	char val = (char)b;
	MPI::COMM_WORLD.Send(&val, 1, MPI::UNSIGNED_CHAR, dest, tag);
}

inline std::string MPI_Recv_string(int source, int tag, MPI::Status &status) {
	MPI::Status s;	
	MPI::COMM_WORLD.Probe(source, tag, s);
	int length = s.Get_count(MPI::UNSIGNED_CHAR);
	
	char* buffer = new char[length];
	MPI::COMM_WORLD.Recv(buffer, length, MPI::UNSIGNED_CHAR, source, tag, status);
	std::string result(buffer);
	delete[] buffer;
	return result;
}

inline std::string MPI_Recv_string(int source, int tag) {
	MPI::Status s;	
	MPI::COMM_WORLD.Probe(source, tag, s);
	int length = s.Get_count(MPI::UNSIGNED_CHAR);
	
	char* buffer = new char[length];
	MPI::COMM_WORLD.Recv(buffer, length, MPI::UNSIGNED_CHAR, source, tag);
	std::string result(buffer);
	delete[] buffer;
	return result;
}

inline bool MPI_Recv_bool(int source, int tag, MPI::Status &status) {
	char val;
	MPI::COMM_WORLD.Recv(&val, 1, MPI::UNSIGNED_CHAR, source, tag, status);
	return (bool)val;
}

inline bool MPI_Recv_bool(int source, int tag) {
	char val;
	MPI::COMM_WORLD.Recv(&val, 1, MPI::UNSIGNED_CHAR, source, tag);
	return (bool)val;
}