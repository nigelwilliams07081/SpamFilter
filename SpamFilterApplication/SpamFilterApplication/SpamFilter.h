#pragma once

#include "Email.h"

class SpamFilter
{
public:
	SpamFilter();

	/**
	: [float]
	: Returns the confidence level that a given message is spam
	*/
	float GetOverallSpamConfidence() const;

	/**
	: [vector<string>]
	: Returns the list of addresses obtained from the spam_email_addresses.txt file
	*/
	std::vector<std::string> GetSpamAddressList() const;

	/**
	: [vector<string>]
	: Returns the list of subjects obtained from the spam_email_subjects.txt file
	*/
	std::vector<std::string> GetSpamSubjectList() const;

	/**
	: [vector<string>]
	: Returns the list of phrases obtained from the spam_words.txt file
	*/
	std::vector<std::string> GetSpamPhraseList() const;

	/**
	: [vector<string>]
	: Returns the list of attachments obtained from the spam_attachments.txt file
	*/
	std::vector<std::string> GetSpamAttachmentList() const;

	/**
	: Sets the Email struct
	*/
	void SetEmail(Email e);

	/**
	: [bool]
	: Attempts to open the file specified by <fileString>
	: returns false if the attempt fails
	*/
	bool OpenFile(std::string fileString);

	/**
	: (No Current Description)
	: (Not Complete)
	*/
	void PerformSpamSearch();

	/**
	: Used for testing to see if the body is lowercase
	*/
	void PrintEmailBody();

	const float POSSIBLE_SPAM_THRESHOLD = 0.4f;
	const float SPAM_THRESHOLD = 0.7f;

private:
	void GrabLinesFromFile(std::string fileString, std::vector<std::string>& spamList);

	void PerformSenderSearch(std::string& sender, const std::string& spamFileName);
	void PerformSubjectSearch(std::string& subject, const std::string& spamFileName);
	void PerformPhraseSearch(std::string& body, const std::string& spamFileName);
	void PerformAttachmentSearch(std::string*& attachments, const std::string& spamFileName);

	std::ifstream m_File;

	std::vector<std::string> m_SpamAddressList;
	std::vector<std::string> m_SpamSubjectList;
	std::vector<std::string> m_SpamPhraseList;
	std::vector<std::string> m_SpamAttachmentList;

	float m_SpamAddressConfidence;
	float m_SpamSubjectConfidence;
	float m_SpamPhraseConfidence;
	float m_SpamAttachmentConfidence;
	float m_OverallSpamConfidence;

	Email m_Email;
};