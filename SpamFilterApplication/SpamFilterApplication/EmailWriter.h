#pragma once
#ifndef _EMAILWRITER_H
#define _EMAILWRITER_H

#include <mutex>
#include <fstream>
#include "Email.h"

class EmailWriter {
private:
	std::mutex m_lock;
	std::ofstream m_file;
	void close();
public:
	EmailWriter();
	~EmailWriter();
	EmailWriter(const char*);
	void useFile(const char*);
	void add(Email);
};

#endif