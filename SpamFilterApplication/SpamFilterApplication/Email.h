#pragma once
#ifndef _EMAIL_H
#define _EMAIL_H

struct Email {
	bool IsValid = false;
	std::string Sender;
	std::string Subject;
	std::string Body;
	unsigned int NumAttachments;
	std::string *Attachments;
	float SpamPercentage;
};

#endif