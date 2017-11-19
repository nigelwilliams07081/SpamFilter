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
	FileNames fileNames;

	if (m_Email.IsValid)
	{
		PerformSenderSearch(m_Email.Sender, fileNames.SpamSenderAddressesFile);
		PerformPhraseSearch(m_Email.Body, fileNames.SpamWordsFile);
		PerformAttachmentSearch(m_Email.Attachments, fileNames.SpamAttachmentsFile);
	}
	else
	{
		std::cout << "The email object is null" << std::endl;
	}
	
}

void SpamFilter::PrintEmailBody()
{
	for (int i = 0; i < std::string(m_Email.Body).size(); i++)
	{
		std::cout << m_Email.Body[i];
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
void SpamFilter::PerformSenderSearch(const char sender[256], const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamAddressList);

	for (int i = 0; i < m_SpamAddressList.size(); i++)
	{
		if (sender == m_SpamAddressList.data()[i])
		{
			std::cout << m_SpamAddressList.data()[i] << std::endl;
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
void SpamFilter::PerformSubjectSearch(const char subject[998], const std::string& spamFileName)
{
	printf("%s\t%s", subject, spamFileName);
	// TODO
}

/**
: Search through the spam-phrase file <spamFileName> and see if any words/phrases in that list match the words/phrases in the <body>
: 40% of the algorithm w/ attachment
: 55% of the algorithm w/0 attachment
*/
void SpamFilter::PerformPhraseSearch(const char body[65535], const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamPhraseList);

	std::string emailBody = std::string(body);
	for (int i = 0; i < m_SpamPhraseList.size(); i++)
	{
		// checks the whole Body to see if it finds the m_PhraseList.data()[i] string
		if (emailBody.find(m_SpamPhraseList.data()[i]) != std::string::npos)
		{
			std::cout << m_SpamPhraseList.data()[i] << std::endl;
			if (m_NumberOfAttachments > 0)
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
void SpamFilter::PerformAttachmentSearch(const char attachments[10][255], const std::string& spamFileName)
{
	std::string attachmentName;
	size_t position = 0, found = 0;

	GrabLinesFromFile(spamFileName, m_SpamAttachmentList);

	for (int i = 0; i < m_SpamAttachmentList.size(); i++)
	{
		for (int j = 0; j < m_NumberOfAttachments; j++)
		{
			if (attachments[j] != "")
			{
				attachmentName = std::string(attachments[j]);
				if (attachmentName.find(m_SpamAttachmentList.data()[i]) != std::string::npos)
				{
					std::cout << attachmentName << " is most likely a spam attachment" << std::endl;
					m_SpamAttachmentConfidence = 0.2f;
				}
				else
				{
					std::cout << attachmentName << " is not a spam attachment" << std::endl;
				}
			}
		}
	}
}
