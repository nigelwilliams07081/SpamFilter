#pragma once
#include "Email.h"

class Worker {
private:
	Worker();
	static unsigned int m_nonce;
	static bool *m_workingThreads;
	static void processEmail(Email, int);
public:
	static void mainLoop(int);
};