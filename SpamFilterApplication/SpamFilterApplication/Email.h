#pragma once

struct Email
{
	int IsValid = 0;
	char Sender[256];
	char Subject[998];
	char Body[65535];
	char Attachments[10][255];
};

MPI_Datatype MPI_Email;

const int nitems = 5;

int blocklengths[nitems];
blocklengths[0] = 1;
blocklengths[1] = 256;
blocklengths[2] = 998;
blocklengths[3] = 65535;
blocklengths[4] = 10*255;

MPI_Datatype types[nitems] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_CHAR, MPI_CHAR };

MPI_Aint offsets[nitems];
offsets[0] = offsetof(Email, isValid);
offsets[1] = offsetof(Email, Sender);
offsets[2] = offsetof(Email, Subject);
offsets[3] = offsetof(Email, Body);
offsets[4] = offsetof(Email, Attachments);

MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_Email);
MPI_Type_commit(&MPI_Email);