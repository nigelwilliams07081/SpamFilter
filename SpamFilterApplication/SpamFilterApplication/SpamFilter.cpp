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
	EmailReceiver emailReceiver;
	emailReceiver.RetrieveEmail();

	FileNames fileNames;

	if (emailReceiver.GetEmail().IsValid)
	{
		PerformSenderSearch(emailReceiver.GetEmail().Sender, fileNames.SpamSenderAddressesFile);
		PerformPhraseSearch(emailReceiver.GetEmail().Body, fileNames.SpamWordsFile);
	}
	else
	{
		std::cout << "The email object is null" << std::endl;
	}
	
}

void SpamFilter::NotifyUserOfPossibleSpam()
{
	// TODO
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
*/
void SpamFilter::PerformSenderSearch(const char sender[256], const std::string& spamFileName)
{
	UNREFERENCED_PARAMETER(sender);
	UNREFERENCED_PARAMETER(spamFileName);


	// TODO

	GrabLinesFromFile(spamFileName, m_SpamAddressList);

	for (int i = 0; i < m_SpamAddressList.size(); i++)
	{
		if (sender == m_SpamAddressList.data()[i])
		{
			std::cout << m_SpamAddressList.data()[i] << std::endl;
			if (m_SpamAddressConfidence <= 0.1f)
			{
				m_SpamAddressConfidence += 0.01f;
			}
		}
	}

	m_OverallSpamConfidence += m_SpamAddressConfidence;
}

/**
: Search through the subject file <spamFileName> and see if the <subject> is in that file
*/
void SpamFilter::PerformSubjectSearch(const char subject[998], const std::string& spamFileName)
{
	UNREFERENCED_PARAMETER(subject);
	UNREFERENCED_PARAMETER(spamFileName);
	// TODO
}

/**
: Search through the spam-phrase file <spamFileName> and see if any words/phrases in that list match the words/phrases in the <body>
*/
void SpamFilter::PerformPhraseSearch(const char body[65535], const std::string& spamFileName)
{
	GrabLinesFromFile(spamFileName, m_SpamPhraseList);

	for (int i = 0; i < m_SpamPhraseList.size(); i++)
	{
		// checks the whole Body to see if it finds the m_PhraseList.data()[i] string
		if (std::string(body).find(m_SpamPhraseList.data()[i]) != std::string::npos)
		{
			std::cout << m_SpamPhraseList.data()[i] << std::endl;
			if (m_SpamPhraseConfidence <= 0.1f)
			{
				m_SpamPhraseConfidence += 0.01f;
			}
		}
	}

	m_OverallSpamConfidence += m_SpamPhraseConfidence;
}

/**
: Search through the spam attachment file <spamFileName> and see if any of the <attachments> are in that file
*/
void SpamFilter::PerformAttachmentSearch(const char attachments[10][255], const std::string& spamFileName)
{
	UNREFERENCED_PARAMETER(attachments);
	UNREFERENCED_PARAMETER(spamFileName);

	// TODO

	//std::string tempString;
	//size_t position = 0, found = 0;

	//GrabLinesFromFile(spamFileName, m_SpamAttachmentList);

	//for (int i = 0; i < m_SpamAttachmentList.size(); i++)
	//{
	//	for (int j = 0; j < sizeof(attachments) / sizeof(attachments[0]); j++)
	//	{
	//		if (attachments[j] != "")
	//		{
	//			// found here https://www.quora.com/How-do-I-split-a-string-by-space-into-an-array-in-c++
	//			if (found = std::string(attachments[j]).find_first_of('.', position) != std::string::npos)
	//			{
	//				std::cout << std::string(attachments[j]).substr(position, found - position) << std::endl;
	//			}
	//		}
	//	}
	//}
}
