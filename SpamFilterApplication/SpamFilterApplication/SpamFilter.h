#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class SpamFilter
{
public:
	SpamFilter();

	/**
	: [float]
	: Returns the confidence level that a given message is spam
	*/
	float GetSpamConfidence() const;

	/**
	: [vector<string>]
	: Returns the list of words obtained from previously read file
	*/
	std::vector<std::string> GetWorldList() const;

	/**
	: [vector<string>]
	: Returns the list of phrases obtained from previously read file
	*/
	std::vector<std::string> GetPhraseList() const;

	/**
	: Sets the m_SpamConfidence value
	*/
	void SetSpamConfidence(float confidenceLevel);

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
	: (No Current Description)
	: (Not Complete)
	*/
	void NotifyUserOfPossibleSpam();

	const float POSSIBLE_SPAM_THRESHOLD = 0.3f;
	const float SPAM_THRESHOLD = 0.6f;

private:
	void GrabWordsFromFile(std::string fileString);
	void GrabLinesFromFile(std::string fileString);

	std::ifstream m_File;
	std::vector<std::string> m_WordList;
	std::vector<std::string> m_PhraseList;
	std::vector<std::string> m_SpamKeywordRepository;
	float m_SpamConfidence;
};