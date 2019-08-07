#pragma once
#ifndef _MPI_WRAPPERS_H
#define _MPI_WRAPPERS_H

#include "stdafx.h"

void MPI_Send_string(std::string, int, int);

void MPI_Send_bool(bool, int, int);

std::string MPI_Recv_string(int, int, MPI::Status);
std::string MPI_Recv_string(int, int);

bool MPI_Recv_bool(int, int, MPI::Status);
bool MPI_Recv_bool(int, int);

#endif