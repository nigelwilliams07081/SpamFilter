#include "stdafx.h"
#include "StaticSpamFilter.h"

SSpamFilter::SSpamFilter()
{
}

float SSpamFilter::GetOverallSpamConfidence(float addr, float subj, float phrase, float attachment){
	return addr + subj + phrase + attachment;
}

float SSpamFilter::PerformSpamSearch(Email e)
{
	if (e.IsValid)
	{
		float spamAddressConfidence;
		float spamSubjectConfidence;
		float spamPhraseConfidence;
		float spamAttachmentConfidence;
		
		std::thread phase1(PerformSenderSearch,     std::ref(e), FileNames::SpamSenderAddressesFile, std::ref(spamAddressConfidence));
		std::thread phase2(PerformSubjectSearch,    std::ref(e), FileNames::SpamSubjectFile,         std::ref(spamSubjectConfidence));
		std::thread phase3(PerformPhraseSearch,     std::ref(e), FileNames::SpamWordsFile,           std::ref(spamPhraseConfidence));
		std::thread phase4(PerformAttachmentSearch, std::ref(e), FileNames::SpamAttachmentsFile,     std::ref(spamAttachmentConfidence));
		
		phase1.join();
		phase2.join();
		phase3.join();
		phase4.join();
		
		return GetOverallSpamConfidence(spamAddressConfidence, spamSubjectConfidence, spamPhraseConfidence, spamAttachmentConfidence);
	}
	else
	{
		std::cout << "The email object is null" << std::endl;
		return -1.f;
	}
}

/**
: Grabs a list of information from the <fileString> passed in, and assigns them to the <spamList> variable
*/
void SSpamFilter::GrabLinesFromFile(std::string fileString, std::vector<std::string> &spamList)
{
	std::string tempString;

	std::ifstream file(fileString);
	if (file.is_open())
	{
		while (std::getline(file, tempString))
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
void SSpamFilter::PerformSenderSearch(Email &e, const std::string spamFileName, float &spamAddressConfidence)
{
	std::vector<std::string> spamAddressList;
	GrabLinesFromFile(spamFileName, spamAddressList);

	for (int i = 0; i < spamAddressList.size(); i++)
	{
		if (e.Sender == spamAddressList.data()[i])
		{
			//std::cout << spamAddressList.data()[i] << std::endl;
			spamAddressConfidence = 0.15f;
		}
	}
}

/**
: Search through the subject file <spamFileName> and see if the <subject> is in that file
: 25% of the algorithm w/ attachment
: 30% of the algorithm w/o attachment
*/
void SSpamFilter::PerformSubjectSearch(Email &e, const std::string spamFileName, float &spamSubjectConfidence)
{
	std::vector<std::string> spamSubjectList;
	GrabLinesFromFile(spamFileName, spamSubjectList);

	for (int i = 0; i < spamSubjectList.size(); i++)
	{
		if (e.Subject.find(spamSubjectList.data()[i]) != std::string::npos)
		{
			//std::cout << spamSubjectList.data()[i] << std::endl;
			if (e.NumAttachments > 0)
			{
				spamSubjectConfidence = 0.25f;
			}
			else
			{
				spamSubjectConfidence = 0.3f;
			}
		}
	}
}

/**
: Search through the spam-phrase file <spamFileName> and see if any words/phrases in that list match the words/phrases in the <body>
: 40% of the algorithm w/ attachment
: 55% of the algorithm w/0 attachment
*/
void SSpamFilter::PerformPhraseSearch(Email &e, const std::string spamFileName, float &spamPhraseConfidence)
{
	std::vector<std::string> spamPhraseList;
	GrabLinesFromFile(spamFileName, spamPhraseList);

	for (int i = 0; i < spamPhraseList.size(); i++)
	{
		// checks the whole Body to see if it finds the m_PhraseList.data()[i] string
		if (e.Body.find(spamPhraseList.data()[i]) != std::string::npos)
		{
			//std::cout << spamPhraseList.data()[i] << std::endl;
			if (e.NumAttachments > 0)
			{
				// We will check up to 10 words for full confidence
				if (spamPhraseConfidence < 0.4f)
				{
					spamPhraseConfidence += 0.04f;
				}
			}
			else
			{
				// We will check up to 10 words for full confidence
				if (spamPhraseConfidence < 0.55f)
				{
					spamPhraseConfidence += 0.055f;
				}
			}
		}
	}
}

/**
: Search through the spam attachment file <spamFileName> and see if any of the <attachments> are in that file
: 20% of the algorithm
*/
void SSpamFilter::PerformAttachmentSearch(Email &e, const std::string spamFileName, float &spamAttachmentConfidence)
{
	if (e.NumAttachments == 0) {
		spamAttachmentConfidence = 0;
		return;
	}
	
	size_t position = 0, found = 0;

	std::vector<std::string> spamAttachmentList;
	GrabLinesFromFile(spamFileName, spamAttachmentList);

	for (int i = 0; i < spamAttachmentList.size(); i++)
	{
		for (int j = 0; j < e.NumAttachments; j++)
		{
			if (e.Attachments[j].find(spamAttachmentList.data()[i]) != std::string::npos)
			{
				std::cout << e.Attachments[j] << " is most likely a spam attachment" << std::endl;
				spamAttachmentConfidence = 0.2f;
			}
			else
			{
				std::cout << e.Attachments[j] << " is not a spam attachment" << std::endl;
				spamAttachmentConfidence = 0;
			}
		}
	}
}
