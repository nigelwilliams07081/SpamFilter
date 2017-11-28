#include "stdafx.h"

#include "EmailReader.h"
#include <algorithm>

EmailReader::EmailReader() {
}

EmailReader::~EmailReader() {
	delete[] m_buffer;
}

EmailReader::EmailReader(const char* filename) {
	loadFromFile(filename);
}

Email EmailReader::constructEmail(rapidxml::xml_node<> *emailData) {
	Email result;

	// Note that:
	//    <node>Hello world</node>
	// is a node containing a text node. Therefore, we must do:
	//     root->first_node("node")->first_node()->value()
	// to actually get "Hello world" out. However, each call 
	// returns a temporary pointer and so must be done one at a time.

	auto sender = emailData->first_node("sender");
	sender = sender->first_node();

	auto subject = emailData->first_node("subject");
	subject = subject->first_node();

	auto body = emailData->first_node("body");
	body = body->first_node();
	
	result.Sender  = sender->value();
	result.Subject = subject->value();
	result.Body    = body->value();

	auto attachmentroot = emailData->first_node("attachments");

	// Iterate over children of the attachments node to gather attachments
	int i = 0;
	auto filename = attachmentroot->first_node();
	while (filename != NULL) {
		filename = filename->next_sibling();
		i++;
	}
	
	result.NumAttachments = i;
	result.Attachments = new std::string[i];
	
	i = 0;
	filename = attachmentroot->first_node();
	while (filename != NULL) {
		result.Attachments[i++] = filename->value();
		filename = filename->next_sibling();
	}

	result.IsValid = true;
	result.SpamPercentage = -1.;
	return result;
}

void EmailReader::loadFromFile(const char* filename) {

	// Open file with fstream
	std::ifstream xmlFile;
	xmlFile.open(filename);

	// Read entire contents of file into memory
	std::stringstream buffer;
	buffer << xmlFile.rdbuf();

	// Copy file contents into char[] buffer
	int len = buffer.str().size();
	m_buffer = new char[len + 1];

	#ifdef WIN32
	// strncpy throws warnings in Windows compilers
	strncpy_s(m_buffer, len + 1, buffer.str().c_str(), len);
	#else
	strncpy(m_buffer, buffer.str().c_str(), len);
	#endif

	m_buffer[len] = '\0'; // Don't forget to wear your null terminator

	// Free resources
	xmlFile.close();
	buffer.clear();
	buffer.str(std::string());

	// Pass file contents to XML parser
	m_emailDocument.clear();
	m_emailDocument.parse<0>(m_buffer);

	// Get the root node of the XML tree (should always be emailList)
	auto rootNode = m_emailDocument.first_node("emailList");

	// Get the first and last nodes for iterator
	m_beginning = rootNode->first_node();
	m_ending = rootNode->last_node();
	
	for (auto node = m_beginning; node != NULL; node = node->next_sibling()) {
		m_emailCount++;
	}
	
	if (m_emailCount > 1) {
		m_currentEmail = m_beginning->next_sibling();
		m_currentOffset = 1;
	}
}

int EmailReader::getEmailCount() {
	return m_emailCount;
}

Email EmailReader::get(int distanceFromStart) const {
	
	// Out of bounds
	if (distanceFromStart < 0 || distanceFromStart >= m_emailCount) {
		printf("Bad email read: %i\n", distanceFromStart);
		return *(Email*)NULL; // Yuck, don't make anything run this
	}
	
	// Directly get the first last, and current
	if (distanceFromStart == 0)
		return constructEmail(m_beginning);
		
	if (distanceFromStart == m_emailCount - 1)
		return constructEmail(m_ending);
	
	
	// Compute how to find the pointer to the requested element the fastest
	int distanceFromEnd = abs(m_emailCount - distanceFromStart);
	
	if (distanceFromStart < distanceFromEnd) {
		auto node = m_beginning;
		
		// Traverse from the start until we reach the target
		for (int i = 0; i < distanceFromStart; i++) {
			node = node->next_sibling();
		}
		
		return constructEmail(node);
	} else {
		auto node = m_ending;
		
		// Traverse from the end until we reach the target
		for (int i = 0; i < distanceFromEnd; i++) {
			node = node->previous_sibling();
		}
		
		return constructEmail(node);
	}
}