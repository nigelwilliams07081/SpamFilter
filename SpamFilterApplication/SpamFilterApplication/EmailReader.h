#pragma once
#include "rapidxml.hpp"

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
	: Skip forward one place in the iterator.
	*/
	void advance();

	/**
	: Skip forward or backward X places in the iterator.
	*/
	void advance(int);

	/**
	: Used for resetting to the start of the iterator. Calling next() after this
	: will return the first email.
	*/
	void begin();

	/**
	: Used for jumping to the very end of the iterator. Calling prev() after this
	: will return the last email.
	*/
	void end();

	/**
	: [Email]
	: Return the next email from the iterator.
	*/
	Email next();

	/**
	: [Email]
	: Return the previous email from the iterator.
	*/
	Email prev();

	/**
	: [bool]
	: Returns true if there is at least one more email in the iterator.
	*/
	bool hasNext();

	/**
	: [bool]
	: Returns true if there is at least one previous email in the iterator.
	*/
	bool hasPrev();
};