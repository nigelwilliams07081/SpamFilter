#pragma once
#include "eagetmailobj.tlh"
#include "rapidxml.hpp"
#include <atlstr.h>

struct Email
{
	bool IsValid = false;
	char Sender[256];
	char Subject[998];
	char Body[65535];
	char Attachments[10][255];
};

class EmailReceiver
{
public:
	EmailReceiver();

	/**
	: You must enable the "Allow Less Secure Apps" Option in your Gmail account for this to work
	*/
	void RetrieveEmail();
	const bool HasRetrievedEmail();
	const bool HasAttachment();
	const int NumberOfAttachments();
	const Email GetEmail();

private:
	void CreateLocalInboxFolder(TCHAR& mailbox);
	void PrintNumberOfEmails(_variant_t& mailinfo, SAFEARRAY*& safeArray, long& lBound, long& uBound);
	void ConnectToServer(EAGetMailObjLib::IMailServerPtr& server);
	void ConnectClientToServer(EAGetMailObjLib::IMailClientPtr& client, EAGetMailObjLib::IMailServerPtr& server);

	void AddEmailToFile(EAGetMailObjLib::IMailPtr& mail, std::ofstream& targetFile);
	// for Joel
	void AddEmailToXMLFile(EAGetMailObjLib::IMailPtr& mail, const char* xmlFileName);
	void SetEmail(EAGetMailObjLib::IMailPtr& mail);

	Email m_Email;
	bool m_HasRetrievedEmail;
	bool m_HasAttachment;
	int m_NumberOfAttachments;
};