#pragma once
#include "eagetmailobj.tlh"
#include <atlstr.h>

class EmailReceiver
{
public:
	EmailReceiver();

	/**
	: You must enable the "Allow Less Secure Apps" Option in your Gmail account for this to work
	*/
	void RetrieveEmail();
private:
	void InitializeInfo(TCHAR& mailbox);
	void InitializeMailArrayInfo(_variant_t& mailinfo, SAFEARRAY*& safeArray, long& lBound, long& uBound);
	void SetEmailServerInfo(EAGetMailObjLib::IMailServerPtr& server);
	void SetEmailClientInfo(EAGetMailObjLib::IMailClientPtr& client, EAGetMailObjLib::IMailServerPtr& server);

	void AddToFileFromEmail(EAGetMailObjLib::IMailPtr& mail, std::ofstream& targetFile);
};