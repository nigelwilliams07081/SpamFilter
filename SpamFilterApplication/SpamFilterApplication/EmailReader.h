#pragma once
#include "rapidxml.hpp"

#define EMAILREADER_EXPERIMENT
// Experimental changes to the email reader class to make it more thread-friendly
// Iterator pattern is replaced with simple access

class EmailReader {

private:

	rapidxml::xml_document<> m_emailDocument;

	// Note: RapidXML just creates pointers to this value, so if it is ever deallocated
	// all the pointers will point to garbage. It is REQUIRED that this char array be here,
	// and not a pointer to one or a std::string. However you can adjust the buffer size 
	// to fit the file.
	char m_buffer[65536];

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
	EmailReader(const char*);

	/**
	: Load an email iterator from an xml file. Will throw exceptions from rapidxml
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
	Email get(int);
	
};