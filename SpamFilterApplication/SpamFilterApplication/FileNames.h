#pragma once

#include <string>

/**
: Contains the names of all the text files that will be used in the application
*/
struct FileNames {
	const std::string SpamSenderAddressesFile = "spam_email_addresses.txt";
	const std::string SpamSubjectFile = "spam_subjects.txt";
	const std::string SpamWordsFile = "spam_words.txt";
	const std::string SpamAttachmentsFile = "spam_attachments.txt";
};