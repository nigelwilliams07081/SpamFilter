#pragma once
#include "Email.h"

class Worker {
private:
	Worker();
	static unsigned int m_nonce;
	static void processEmail(Email);
public:
	static void mainLoop();
};