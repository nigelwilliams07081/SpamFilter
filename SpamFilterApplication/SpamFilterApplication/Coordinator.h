#pragma once
#include "EmailReader.h"

class Coordinator {
private:
	static EmailReader reader;
	
	static int m_emailsSent;
	static int m_totalEmails;
	static int m_repliesReceived;
	
	static bool m_finished;
	
	Coordinator();
	static void talkWithNode(int);
	static void receiveResult();
public:
	static void mainLoop(const char*);
};

