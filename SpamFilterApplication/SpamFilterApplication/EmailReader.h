#pragma once
#ifndef _EMAILREADER_H
#define _EMAILREADER_H

#include "rapidxml.hpp"

class EmailReader {

private:

	rapidxml::xml_document<> m_emailDocument;
	
	char *m_buffer;

	rapidxml::xml_node<> *m_currentEmail;
	rapidxml::xml_node<> *m_beginning;
	rapidxml::xml_node<> *m_ending;
	
	int m_emailCount;
	int m_currentOffset;

	/**
	: [Email]
	: Static utility function - will convert an xml_node of an <email> tag to an email
	: structure.
	*/
	static Email constructEmail(rapidxml::xml_node<>*);

public:
	EmailReader();
	~EmailReader();
	
	EmailReader(const char*);

	/**
	: Load an email from an xml file. Will throw exceptions from rapidxml
	: if the xml file is not formatted as expected.
	*/
	void loadFromFile(const char*);

	/**
	: [int]
	: Returns the total number of emails. Will return 0 if no file has been loaded.
	*/
	int getEmailCount();
	
	
	/**
	: [Email]
	: Returns the email at position X. Returns NULL for out-of-bounds values.
	*/
	Email get(int) const;
	
};

#endif