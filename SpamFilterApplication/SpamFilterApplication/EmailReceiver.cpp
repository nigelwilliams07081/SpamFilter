
#include "stdafx.h"

using namespace EAGetMailObjLib;

EmailReceiver::EmailReceiver()
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
				wsprintf(nameOfFile, L"%s\\%04d%02d%02d%02d%02d%02d%03d%d.eml", sizeOfMailBox,
					currentTime.wYear, currentTime.wMonth, currentTime.wDay, currentTime.wHour,
					currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds, i);

				IMailPtr outMail = outClient->GetMail(pInfo);

				// Passes the email information to the Email struct
				SetEmail(outMail);

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
		wprintf(L"Error: %s", (const TCHAR*)errorParameter.Description());
	}
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
	wsprintf(&mailbox, L"%s\\inbox", nameOfPath);

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

void EmailReceiver::AddToFileFromEmail(IMailPtr & mail, std::ofstream & targetFile)
{
	if (mail->IsEncrypted == VARIANT_TRUE)
	{
		mail = mail->Decrypt(NULL);
	}

	if (mail->IsSigned == VARIANT_TRUE)
	{
		ICertificatePtr certificate = mail->VerifySignature();
		_tprintf(L"This email contains a valid digital signature.\r\n");
	}

	mail->DecodeTNEF();

	// Put the name, email address, and text body of the email into the targetFile
	targetFile << mail->From->Name << "\n" << mail->From->Address << "\n" <<
		mail->GetTextBody() << "\n\n";
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
		_tprintf(L"This email contains a valid digital signature.\r\n");
	}

	mail->DecodeTNEF();

	strcpy_s(m_Email.Sender, _com_util::ConvertBSTRToString(mail->From->Address));
	strcpy_s(m_Email.Subject, _com_util::ConvertBSTRToString(mail->Subject));
	strcpy_s(m_Email.Body, _com_util::ConvertBSTRToString(mail->TextBody));
	
	if (mail->GetAttachmentList() != nullptr)
	{
		// Assign the name of the attachments in the email
		for (int i = 0; i < mail->GetAttachmentList()->Length; i++)
		{
			strcpy_s(m_Email.Attachments[i], _com_util::ConvertBSTRToString(mail->GetAttachmentList()->GetItem(i)->Name));
		}
		// Assign an empty string to the rest of the attachments
		for (int i = mail->GetAttachmentList()->Length; i < sizeof(m_Email.Attachments) / sizeof(m_Email.Attachments[0]); i++)
		{
			strcpy_s(m_Email.Attachments[i], "");
		}
	}

	// Used for making sure the Email struct is not null
	m_Email.IsValid = true;
}

const Email EmailReceiver::GetEmail()
{
	return m_Email;
}
