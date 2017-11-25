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

// Unimplemented, may be useful later
// Reason for not being implemented is the way the tags are handled

/*
void MPI_Send_Email(Email e, int dest) {
	
	MPI_Send_string(e.Sender,  dest, TAG_RETURN_EMAIL_SENDER);
	MPI_Send_string(e.Subject, dest, TAG_RETURN_EMAIL_SUBJECT);
	MPI_Send_string(e.Body,    dest, TAG_RETURN_EMAIL_BODY);
	
	MPI::COMM_WORLD.Send(&(e.SpamPercentage), 1, MPI::FLOAT, dest, TAG_RETURN_EMAIL_SPAM_PERCENTAGE);
	MPI::COMM_WORLD.Send(&(e.NumAttachments), 1, MPI::INT,   dest, TAG_RETURN_EMAIL_NUM_ATTACHMENTS);
	
	for (int i = 0; i < e.NumAttachments; i++) {
		MPI_Send_string(e.Attachments[i], dest, TAG_RETURN_EMAIL_ATTACHMENT);
	}
}

Email MPI_Recv_Email(int source);
	
	Email result;
	MPI::Status status;
	
	result.Sender  = MPI_Recv_string(source, TAG_RETURN_EMAIL_SENDER, status);
	
	if (source == MPI::ANY_SOURCE) {
		source = status.Get_source();
	}
	
	result.Subject = MPI_Recv_string(source, TAG_RETURN_EMAIL_SUBJECT);
	result.Body    = MPI_Recv_string(source, TAG_RETURN_EMAIL_BODY);
	
	float spamPercent;
	MPI::COMM_WORLD.Recv(&spamPercent, 1, MPI::FLOAT, source, TAG_RETURN_EMAIL_SPAM_PERCENTAGE);
	result.SpamPercentage = spamPercent;
	
	MPI::COMM_WORLD.Recv(&(result.NumAttachments), 1, MPI::INT, source, TAG_RETURN_EMAIL_NUM_ATTACHMENTS);
	
	result.Attachments = new std::string[result.NumAttachments];
	for (int i = 0; i < result.NumAttachments; i++) {
		result.Attachments[i] = MPI_Recv_string(source, TAG_RETURN_EMAIL_ATTACHMENT);
	}
	
	return result;
}