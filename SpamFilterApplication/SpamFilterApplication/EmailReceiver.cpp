
#include "stdafx.h"

using namespace EAGetMailObjLib;

EmailReceiver::EmailReceiver()
{
	
}

void EmailReceiver::RetrieveEmail()
{
	
	TCHAR sizeOfMailBox[MAX_PATH + 1];
	InitializeInfo(*sizeOfMailBox);

	try
	{
		IMailServerPtr outServer = NULL;
		IMailClientPtr outClient = NULL;

		SetEmailServerInfo(outServer);
		SetEmailClientInfo(outClient, outServer);
		
		_variant_t info = outClient->GetMailInfos();
		SAFEARRAY *pSafeArray = info.parray;
		long LBound = 0, UBound = 0;

		InitializeMailArrayInfo(info, pSafeArray, LBound, UBound);

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

				TCHAR sizeOfFile[MAX_PATH + 1];
				memset(sizeOfFile, 0, sizeof(sizeOfFile));

				SYSTEMTIME currentTime;
				GetLocalTime(&currentTime);
				wsprintf(sizeOfFile, L"%s\\%04d%02d%02d%02d%02d%02d%03d%d.eml", sizeOfMailBox,
					currentTime.wYear, currentTime.wMonth, currentTime.wDay, currentTime.wHour,
					currentTime.wMinute, currentTime.wSecond, currentTime.wMilliseconds, i);

				IMailPtr outMail = outClient->GetMail(pInfo);

				AddToFileFromEmail(outMail, textFile);

				outMail->SaveAs(sizeOfFile, VARIANT_TRUE);

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

void EmailReceiver::InitializeInfo(TCHAR & mailbox)
{
	CoInitialize(NULL);

	TCHAR sizeOfPath[MAX_PATH + 1];
	memset(sizeOfPath, 0, sizeof(sizeOfPath));
	GetModuleFileName(NULL, sizeOfPath, MAX_PATH);

	LPCTSTR pathSize = _tcsrchr(sizeOfPath, L'\\');

	if (pathSize != NULL)
	{
		sizeOfPath[pathSize - sizeOfPath] = L'\0';
	}

	memset(&mailbox, 0, sizeof(mailbox));
	wsprintf(&mailbox, L"%s\\inbox", sizeOfPath);

	CreateDirectory(&mailbox, NULL);
}

void EmailReceiver::InitializeMailArrayInfo(_variant_t & mailInfo, SAFEARRAY* & safeArray, long & lBound, long & uBound)
{
	SafeArrayGetLBound(safeArray, 1, &lBound);
	SafeArrayGetUBound(safeArray, 1, &uBound);

	INT count = uBound - lBound + 1;
	wprintf(L"Total %d emails\r\n", count);
}

void EmailReceiver::SetEmailServerInfo(IMailServerPtr & server)
{
	server.CreateInstance("EAGetMailObj.MailServer");
	server->Server = L"pop.gmail.com";
	server->User = L"cyberthreat1234@gmail.com";
	server->Password = L"paralleldistributed";
	//outServer->Protocol = MailServerPop3;

	server->SSLConnection = VARIANT_TRUE;
	server->Port = 995;
}

void EmailReceiver::SetEmailClientInfo(IMailClientPtr & client, IMailServerPtr & server)
{
	client.CreateInstance("EAGetMailObj.MailClient");
	client->LicenseCode = L"TryIt";

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

	targetFile << mail->From->Name << "\n" << mail->From->Address << "\n" <<
		mail->GetTextBody() << "\n\n";
}
