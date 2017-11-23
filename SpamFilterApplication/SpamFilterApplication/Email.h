#pragma once

// Original Email struct
struct Email
{
	int IsValid = 0;
	char Sender[256];
	char Subject[998];
	char Body[65535];
	char Attachments[10][255];
	float SpamPercentage;
};

// Create 'attachment' type, consisting of 255 contiguous chars
MPI_Datatype MPI_Email_attachment;
MPI_Type_contiguous(255, MPI_CHAR, &MPI_Email_attachment);
MPI_Type_commit(&MPI_Email_attachment);


// Create 'Email' type
MPI_Datatype MPI_Email;

MPI_Datatype types[] = {
	MPI_INT, 
	MPI_CHAR,
	MPI_CHAR, 
	MPI_CHAR, 
	MPI_Email_attachment,
	MPI_FLOAT
};

int blocklengths[] = {
	1,
	256,
	998,
	65535,
	10,
	1
};

MPI_Aint offsets[] {
	offsetof(Email, isValid),
	offsetof(Email, Sender),
	offsetof(Email, Subject),
	offsetof(Email, Body),
	offsetof(Email, Attachments),
	offsetof(Email, SpamPercentage)
}

MPI_Type_create_struct(6, blocklengths, offsets, types, &MPI_Email);
MPI_Type_commit(&MPI_Email);