#include "stdafx.h"
#include "EmailReader.h"

EmailReader::EmailReader() {
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

	// Use strcpy to copy data out
	strcpy_s(result.Sender, sender->value());
	strcpy_s(result.Subject, subject->value());
	strcpy_s(result.Body, body->value());

	auto attachmentroot = emailData->first_node("attachments");

	// Iterate over children of the attachments node to gather attachments
	int i = 0;
	for (auto filename = attachmentroot->first_node(); filename != NULL; filename = filename->next_sibling()) {
		strcpy_s(result.Attachments[i], filename->value());

		// Break out of the loop after 10 attachments - struct limit
		if (++i == 10) {
			break;
		}
	}

	result.IsValid = true;
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
	strcpy_s(m_buffer, buffer.str().c_str());

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

	m_currentEmail = rootNode->first_node();
}

void EmailReader::advance() {
	m_currentEmail = m_currentEmail->next_sibling();
}

void EmailReader::advance(int amt) {

	if (amt == 0) {
		return;
	}

	if (amt < 0) {
		amt *= -1;
		for (int i = 0; i < amt; i++) {
			m_currentEmail = m_currentEmail->previous_sibling();
		}
	} else {
		for (int i = 0; i < amt; i++) {
			m_currentEmail = m_currentEmail->next_sibling();
		}
	}
}

void EmailReader::begin() {
	m_currentEmail = m_beginning;
}

void EmailReader::end() {
	m_currentEmail = NULL;
}

Email EmailReader::next() {
	Email result = constructEmail(m_currentEmail);
	m_currentEmail = m_currentEmail->next_sibling();
	return result;
}

Email EmailReader::prev() {

	// End of list, going from NULL back to the last email is 
	// basically going to the previous one from off the end
	if (m_currentEmail == NULL) {
		m_currentEmail = m_ending;
	} else {
		m_currentEmail = m_currentEmail->previous_sibling();
	}

	return constructEmail(m_currentEmail);
}

bool EmailReader::hasNext() {
	return m_currentEmail != NULL;
}

bool EmailReader::hasPrev() {
	return m_currentEmail->previous_sibling() != NULL;
}