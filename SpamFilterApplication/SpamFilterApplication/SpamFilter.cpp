#include "stdafx.h"

SpamFilter::SpamFilter() :
	m_SpamConfidence(0.0f)
{
}

float SpamFilter::GetSpamConfidence() const
{
	return m_SpamConfidence;
}

std::vector<std::string> SpamFilter::GetWorldList() const
{
	return m_WordList;
}

std::vector<std::string> SpamFilter::GetPhraseList() const
{
	return m_PhraseList;
}

void SpamFilter::SetSpamConfidence(float confidenceLevel)
{
	m_SpamConfidence = confidenceLevel;
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
}

void SpamFilter::NotifyUserOfPossibleSpam()
{
}

/**
: Grabs a list of words from the <fileString> passed in, and assigns to the m_WordList variable
*/
void SpamFilter::GrabWordsFromFile(std::string fileString)
{
	std::string tempString;

	OpenFile(fileString);
	if (m_File.is_open())
	{
		while (m_File >> tempString)
		{
			m_WordList.push_back(tempString);
		}
	}
	else
	{
		printf("%s", "Can't perform spam search. File is not open");
	}
}

/**
: (Future) Grabs a list of phrases from the <fileString> passed in, and assigns to the m_PhraseList variable
*/
void SpamFilter::GrabLinesFromFile(std::string fileString)
{

}