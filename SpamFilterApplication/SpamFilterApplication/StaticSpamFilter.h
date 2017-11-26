#pragma once

#include "Email.h"

class SSpamFilter
{
public:

	static float PerformSpamSearch(Email e);

private:

	SSpamFilter();

	/**
	: [float]
	: Returns the confidence level that a given message is spam
	*/
	static float GetOverallSpamConfidence(float, float, float, float);
	
	static void GrabLinesFromFile(std::string fileString, std::vector<std::string>& spamList);

	static void PerformSenderSearch(    Email &e, const std::string spamFileName, float &answer);
	static void PerformSubjectSearch(   Email &e, const std::string spamFileName, float &answer);
	static void PerformPhraseSearch(    Email &e, const std::string spamFileName, float &answer);
	static void PerformAttachmentSearch(Email &e, const std::string spamFileName, float &answer);
};