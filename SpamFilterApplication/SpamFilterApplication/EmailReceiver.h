#pragma once
#include "eagetmailobj.tlh"

class EmailReceiver
{
public:
	EmailReceiver();

	/**
	: You must enable the "Allow Less Secure Apps" Option in your Gmail account for this to work
	*/
	void RetrieveEmail();
private:

};