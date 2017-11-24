#pragma once

//struct Email
//{
//	int IsValid = 0;
//	char Sender[256];
//	char Subject[998];
//	char Body[65535];
//	char Attachments[10][255];
//	float SpamPercentage;
//};

struct Email {
	bool IsValid = false;
	std::string Sender;
	std::string Subject;
	std::string Body;
	int NumAttachments;
	std::string *Attachments;
	float SpamPercentage;
};