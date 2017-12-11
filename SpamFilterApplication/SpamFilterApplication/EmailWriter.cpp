#include "stdafx.h"

EmailWriter::EmailWriter() {
}

EmailWriter::~EmailWriter() {
	close();
}

EmailWriter::EmailWriter(const char *filename) {
	useFile(filename);
}

void EmailWriter::useFile(const char *filename) {
	close();
	
	m_file.open(filename);
	m_file << "<emailList>";
}

void EmailWriter::close() {
	if (m_file.is_open()) {
		m_lock.lock();
		m_file << "</emailList>";
		m_lock.unlock();
		m_file.close();
	}
}

void EmailWriter::add(Email e) {
	
	m_lock.lock();
	
	std::string spamstatus;
	if (e.SpamPercentage < 0.4f) 
		spamstatus = "no";
	else if (e.SpamPercentage > 0.4f && e.SpamPercentage < 0.7f) 
		spamstatus = "maybe";
	else 
		spamstatus = "yes";
	
	m_file 
	  << "<email spam=\"" << spamstatus << "\">" 
	  << "<subject>"      << e.Subject  << "</subject>"
	  << "<sender>"       << e.Sender   << "</sender>" 
	  << "<body>"         << e.Body     << "</body>"
	  << "<attachments>";
	  
	for (unsigned int i = 0; i < e.NumAttachments; i++) {
		m_file << "<file>" << e.Attachments[i] << "</file>";
	}
	
	m_file 
	  << "</attachments>"
	  << "</email>";
	
	m_lock.unlock();
}