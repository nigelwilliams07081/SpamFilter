#include "stdafx.h"

SpamFilter::SpamFilter() :
	m_OverallSpamConfidence(0.0f), m_SpamAddressConfidence(0.0f), m_SpamSubjectConfidence(0.0f),
	m_SpamPhraseConfidence(0.0f), m_SpamAttachmentConfidence(0.0f)
{
}

float SpamFilter::GetOverallSpamConfidence() const
{
	return m_OverallSpamConfidence;
}

std::vector<std::string> SpamFilter::GetSpamAddressList() const
{
	return m_SpamAddressList;
}

std::vector<std::string> SpamFilter::GetSpamSubjectList() const
{
	return m_SpamSubjectList;
}

std::vector<std::string> SpamFilter::GetSpamPhraseList() const
{
	return m_SpamPhraseList;
}

std::vector<std::string> SpamFilter::GetSpamAttachmentList() const
{
	return m_SpamAttachmentList;
}

void SpamFilter::SetEmail(Email e)
{
	m_Email = e;
}

bool SpamFilter::OpenFile(std::string fileString)
{
	if (m_File.is_open())
	{
		m_File.close();
	}

	m_File.open(fileString);

	if (m_File.fail())
	{
		printf("%s\n", "Could not open file");
		return false;
	}

	return true;
}

void SpamFilter::PerformSpamSearch()
{
	if (m_Email.IsValid)
	{
		PerformSenderSearch(m_Email.Sender, FileNames::SpamSenderAddressesFile);
		PerformSubjectSearch(m_Email.Subject, FileNames::SpamSubjectFile);
		PerformPhraseSearch(m_Email.Body, FileNames::SpamWordsFile);
		PerformAttachmentSearch(m_Email.Attachments, FileNames::SpamAttachmentsFile);
	}
	else
	{
		std::cout << "The email object is null" << std::endl;
	}
	
}

void SpamFilter::PrintEmailBody()
{
	for (int i = 0; i < m_Email.Body.size(); i++)
	{
		std::cout << m_Email.Body;
	}
}

/**
: Grabs a list of information from the <fileString> passed in, and assigns them to the <spamList> variable
*/
void SpamFilter::GrabLinesFromFile(std::string fileString, std::vector<std::string>& spamList)
{
	std::string tempString;

	OpenFile(fileString);
	if (m_File.is_open())
	{
		while (std::getline(m_File, tempString))
		{
			spamList.push_back(tempString);
		}
	}
	else
	{
		printf("%s", "Can't perform spam search. File is not open");
	}
}

/**
: Search through the sender Address file <spamFileName> and see if the <sender> is in that file
: 15% of the algorithm w/ and w/o attachment
*/
void SpamFilter::PerformSenderSearch(std::string & sender, const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamAddressList);

	for (int i = 0; i < m_SpamAddressList.size(); i++)
	{
		if (sender == m_SpamAddressList.data()[i])
		{
			m_SpamAddressConfidence = 0.15f;
		}
	}

	m_OverallSpamConfidence += m_SpamAddressConfidence;
}

/**
: Search through the subject file <spamFileName> and see if the <subject> is in that file
: 25% of the algorithm w/ attachment
: 30% of the algorithm w/o attachment
*/
void SpamFilter::PerformSubjectSearch(std::string & subject, const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamSubjectList);

	for (int i = 0; i < m_SpamSubjectList.size(); i++)
	{
		if (subject.find(m_SpamSubjectList.data()[i]) != std::string::npos)
		{
			if (m_Email.NumAttachments > 0)
			{
				m_SpamSubjectConfidence = 0.25f;
			}
			else
			{
				m_SpamSubjectConfidence = 0.3f;
			}
		}
	}
}

/**
: Search through the spam-phrase file <spamFileName> and see if any words/phrases in that list match the words/phrases in the <body>
: 40% of the algorithm w/ attachment
: 55% of the algorithm w/0 attachment
*/
void SpamFilter::PerformPhraseSearch(std::string & body, const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamPhraseList);

	for (int i = 0; i < m_SpamPhraseList.size(); i++)
	{
		// checks the whole Body to see if it finds the m_PhraseList.data()[i] string
		if (body.find(m_SpamPhraseList.data()[i]) != std::string::npos)
		{
			if (m_Email.NumAttachments > 0)
			{
				// We will check up to 10 words for full confidence
				if (m_SpamPhraseConfidence < 0.4f)
				{
					m_SpamPhraseConfidence += 0.04f;
				}
			}
			else
			{
				// We will check up to 10 words for full convidence
				if (m_SpamPhraseConfidence < 0.55f)
				{
					m_SpamPhraseConfidence += 0.055f;
				}
			}
		}
	}

	m_OverallSpamConfidence += m_SpamPhraseConfidence;
}

/**
: Search through the spam attachment file <spamFileName> and see if any of the <attachments> are in that file
: 20% of the algorithm
*/
void SpamFilter::PerformAttachmentSearch(std::string *& attachments, const std::string& spamFileName)
{
	std::string attachmentName;
	size_t position = 0, found = 0;

	GrabLinesFromFile(spamFileName, m_SpamAttachmentList);

	for (int i = 0; i < m_SpamAttachmentList.size(); i++)
	{
		for (int j = 0; j < m_Email.NumAttachments; j++)
		{
			if (attachments[j] != "")
			{
				if (attachments[j].find(m_SpamAttachmentList.data()[i]) != std::string::npos)
				{
					m_SpamAttachmentConfidence = 0.2f;
				}
			}
		}
	}
}
