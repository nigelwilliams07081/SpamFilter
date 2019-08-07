
#include "stdafx.h"

using namespace EAGetMailObjLib;

EmailReceiver::EmailReceiver() :
	m_NumberOfAttachments(0), m_HasAttachment(false), m_HasRetrievedEmail(false)
{
}

void EmailReceiver::RetrieveEmail()
{
	TCHAR sizeOfMailBox[MAX_PATH + 1];
	CreateLocalInboxFolder(*sizeOfMailBox);

	try
	{
		IMailServerPtr outServer = NULL;
		IMailClientPtr outClient = NULL;
		
		ConnectToServer(outServer);
		ConnectClientToServer(outClient, outServer);
		
		_variant_t info = outClient->GetMailInfos();
		SAFEARRAY *pSafeArray = info.parray;
		long LBound = 0, UBound = 0;
		
		// Tells user how many new emails there are since last checked
		PrintNumberOfEmails(info, pSafeArray, LBound, UBound);
		
		std::ofstream textFile;
		textFile.open("File.txt");
		
		if (textFile.is_open())
		{
			for (long i = LBound; i <= UBound; i++)
			{
				_variant_t variantInfo;
				SafeArrayGetElement(pSafeArray, &i, &variantInfo);

				IMailInfoPtr pInfo;
				variantInfo.pdispVal->QueryInterface(__uuidof(IMailInfo), (void**)&pInfo);

				TCHAR nameOfFile[MAX_PATH + 1];
				memset(nameOfFile, 0, sizeof(nameOfFile));

				SYSTEMTIME currentTime;
				GetLocalTime(&currentTime);

				// Assigns the name of the file to be the "Year,month,day,hour,minute,second,millisecond"
				wsprintf(nameOfFile, LPCSTR("%s\\%04d%02d%02d%02d%02d%02d%03d%d.eml"), sizeOfMailBox,
					currentTime.wYear, currentTime.wMonth, currentTime.wDay, currentTime.wHour,
					currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds, i);

				IMailPtr outMail = outClient->GetMail(pInfo);

				// Passes the email information to the Email struct
				//AddEmailToFile(outMail, textFile);
				AddEmailToXMLFile(outMail, "testXML.xml");

				// Saves the email file
				outMail->SaveAs(nameOfFile, VARIANT_TRUE);

				outClient->Delete(pInfo);
			}
		}
		else
		{
			printf("Could not open file");
		}

		info.Clear();
		outClient->Quit();
	}
	catch (_com_error &errorParameter)
	{
		printf("Error: %s", _com_util::ConvertBSTRToString(errorParameter.Description()));
	}
}

const bool EmailReceiver::HasRetrievedEmail()
{
	return m_HasRetrievedEmail;
}

const bool EmailReceiver::HasAttachment()
{
	return m_HasAttachment;
}

const int EmailReceiver::NumberOfAttachments()
{
	return m_NumberOfAttachments;
}

void EmailReceiver::CreateLocalInboxFolder(TCHAR & mailbox)
{
	CoInitialize(NULL);
	
	TCHAR nameOfPath[MAX_PATH + 1];
	memset(nameOfPath, 0, sizeof(nameOfPath));
	GetModuleFileName(NULL, nameOfPath, MAX_PATH);

	LPCTSTR pathSize = _tcsrchr(nameOfPath, L'\\');

	if (pathSize != NULL)
	{
		nameOfPath[pathSize - nameOfPath] = L'\0';
	}

	memset(&mailbox, 0, sizeof(mailbox));
	wsprintf(&mailbox, LPCSTR("%s\\inbox"), nameOfPath);

	CreateDirectory(&mailbox, NULL);
}

void EmailReceiver::PrintNumberOfEmails(_variant_t & mailInfo, SAFEARRAY* & safeArray, long & lBound, long & uBound)
{
	SafeArrayGetLBound(safeArray, 1, &lBound);
	SafeArrayGetUBound(safeArray, 1, &uBound);

	INT count = uBound - lBound + 1;
	wprintf(L"Total %d emails\r\n", count);
}

void EmailReceiver::ConnectToServer(IMailServerPtr & server)
{
	// Creates instance of email server
	server.CreateInstance("EAGetMailObj.MailServer");
	// Sets server information (gmail, username, password)
	server->Server = L"pop.gmail.com";
	server->User = L"cyberthreat1234@gmail.com";
	server->Password = L"paralleldistributed";
	//outServer->Protocol = MailServerPop3;

	server->SSLConnection = VARIANT_TRUE;
	server->Port = 995;
}

void EmailReceiver::ConnectClientToServer(IMailClientPtr & client, IMailServerPtr & server)
{
	// Creates instance of email client
	client.CreateInstance("EAGetMailObj.MailClient");
	// Assigns license code
	client->LicenseCode = L"TryIt";

	// Connects to the email server
	client->Connect(server);
	wprintf(L"Connected\r\n");
}

void EmailReceiver::AddEmailToFile(IMailPtr & mail, std::ofstream & targetFile)
{
	if (mail->IsEncrypted == VARIANT_TRUE)
	{
		mail = mail->Decrypt(NULL);
	}

	if (mail->IsSigned == VARIANT_TRUE)
	{
		ICertificatePtr certificate = mail->VerifySignature();
		_tprintf(LPCSTR("This email contains a valid digital signature.\r\n"));
	}

	mail->DecodeTNEF();

	// Put the name, email address, and text body of the email into the targetFile
	targetFile <<
		"ADDRESS:\n" << mail->From->Address << "\n\n" <<
		"SUBJECT:\n" << mail->Subject << "\n\n" <<
		"BODY:\n" << mail->TextBody << "\n\n" <<
		"ATTACHMENTS:\n";

	for (int i = 0; i < mail->AttachmentList->Count; i++)
	{
		targetFile << mail->AttachmentList->Item[i]->Name << "\n";
	}

	targetFile << "---------------------------------------------\n\n";
}

void EmailReceiver::AddEmailToXMLFile(EAGetMailObjLib::IMailPtr & mail, const char * xmlFileName)
{
	if (mail->IsEncrypted == VARIANT_TRUE)
	{
		mail = mail->Decrypt(NULL);
	}

	if (mail->IsSigned == VARIANT_TRUE)
	{
		ICertificatePtr certificate = mail->VerifySignature();
		_tprintf(LPCSTR("This email contains a valid digital signature.\r\n"));
	}

	mail->DecodeTNEF();

	// Check if file exists, create and add root element if it does not
	std::ifstream test(xmlFileName);

	// If the target file does not exist, create it with a root node
	if (!test.good())
	{
		std::ofstream file(xmlFileName);
		file << "<emailList></emailList>";
		file.close();
	}
	
	test.close();

	// Stringstream to funnel new contents into
	std::stringstream newcontents;

	// Local function to replace content in strings
	auto string_replace = [](std::string &s, const std::string &toReplace, const std::string &replaceWith)
	{
		size_t loc;
		size_t len = toReplace.length();

		while ((loc = s.find(toReplace)) != std::string::npos) {
			s.replace(loc, len, replaceWith);
		}

		return s;
	};

	std::string sender = mail->From->Address;
	std::string subject = mail->Subject;
	std::string body = mail->TextBody;

	// We have to escape all the control characters
	string_replace(sender, "<", "&lt;");
	string_replace(sender, ">", "&gt;");
	string_replace(subject, "<", "&lt;");
	string_replace(subject, ">", "&gt;");
	string_replace(body, "<", "&lt;");
	string_replace(body, ">", "&gt;");

	// Pipe all the new data into the file
	newcontents
	 << "<email>"
	 << "<sender>" << sender << "</sender>"
	 << "<subject>" << subject << "</subject>"
	 << "<body>" << body << "</body>";


	newcontents << "<attachments>";

	// Loop over the attachments and add those
	for (int i = 0; i < mail->AttachmentList->Count; i++)
	{
		if (i == 10) {
			// We can only have 10 attachments, limited by the email struct we must use later
			break;
		}

		std::string name = mail->AttachmentList->Item[i]->Name;

		string_replace(name, "<", "&lt;");
		string_replace(name, ">", "&gt;");

		newcontents << "<file>" << name << "</file>";
	}

	// Close root node back up when we're finished
	newcontents
		<< "</attachments>"
		<< "</email>"
		<< "</emailList>"; 

	// We want to overwrite the last 12 bytes, the '</emailList>', so that we may add new data
	// To do that, we have to use fopen() file handles and seek to 12 bytes before the EOF
	// To write to a fopen() handle we have to have a C-style char array
	std::string temp = newcontents.str();
	const char* dataToBeWritten = temp.c_str();

	FILE* handle;
	fopen_s(&handle, xmlFileName, "r+");
	fseek(handle, -12, SEEK_END);
	
	fwrite(dataToBeWritten, sizeof(char), strlen(dataToBeWritten), handle);

	fclose(handle);
}

void EmailReceiver::SetEmail(EAGetMailObjLib::IMailPtr & mail)
{
	if (mail->IsEncrypted == VARIANT_TRUE)
	{
		mail = mail->Decrypt(NULL);
	}

	if (mail->IsSigned == VARIANT_TRUE)
	{
		ICertificatePtr certificate = mail->VerifySignature();
		_tprintf(LPCSTR("This email contains a valid digital signature.\r\n"));
	}

	mail->DecodeTNEF();

	std::string stringBody = std::string(mail->TextBody);
	size_t bodySize = stringBody.size();

	// Check if any characters are upper case and convert them to lowercase
	for (int i = 0; i < bodySize; i++)
	{
		if (stringBody[i] >= 'A' && stringBody[i] <= 'Z')
		{
			stringBody[i] += ('a' - 'A');
		}
	}

	strcpy_s(m_Email.Sender, _com_util::ConvertBSTRToString(mail->From->Address));
	strcpy_s(m_Email.Subject, _com_util::ConvertBSTRToString(mail->Subject));
	strcpy_s(m_Email.Body, stringBody.c_str());
	
	if (mail->GetAttachmentList() != nullptr)
	{
		if (mail->GetAttachmentList()->Length != 0)
		{
			m_NumberOfAttachments = mail->GetAttachmentList()->Length;
			// Assign the name of the attachments in the email
			for (int i = 0; i < mail->GetAttachmentList()->Length; i++)
			{
				// Checks if the attachment has a name
				if (mail->GetAttachmentList()->Item[i]->Name.length() != 0)
				{
					strcpy_s(m_Email.Attachments[i], _com_util::ConvertBSTRToString(mail->GetAttachmentList()->Item[i]->Name));
					if (!m_HasAttachment)
					{
						m_HasAttachment = true;
					}
				}
			}
		}
		else
		{
			m_HasAttachment = false;
		}
	}
	else
	{
		m_HasAttachment = false;
	}

	// Used for making sure the Email struct is not null
	m_Email.IsValid = true;
}

const Email EmailReceiver::GetEmail()
{
	return m_Email;
}
