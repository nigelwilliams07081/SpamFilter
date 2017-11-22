#pragma once
#include "Email.h"

class Worker {
private:
	Worker();	
	static void processEmail(Email*);
public:
	static void mainLoop();
};