#pragma once
#include "EmailReader.h"
#include "EmailWriter.h"

class Coordinator {
private:
	static EmailReader reader;
	static EmailWriter writer;
	
	static int m_emailsSent;
	static int m_totalEmails;
	static int m_repliesReceived;
	static int m_activeWorkers;
	
	Coordinator();
	static void talkWithNode(int);
	static void receiveResult();
public:
	static void mainLoop(const char*, const char*, bool);
};

