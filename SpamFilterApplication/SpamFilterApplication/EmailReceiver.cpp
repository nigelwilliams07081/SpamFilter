
#include "stdafx.h"

using namespace EAGetMailObjLib;

EmailReceiver::EmailReceiver()
{
	
}

void EmailReceiver::RetrieveEmail()
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

	TCHAR sizeOfMailBox[MAX_PATH + 1];
	memset(sizeOfMailBox, 0, sizeof(sizeOfMailBox));
	wsprintf(sizeOfMailBox, L"%s\\inbox", sizeOfPath);

	CreateDirectory(sizeOfMailBox, NULL);

	try
	{
		IMailServerPtr outServer = NULL;
		outServer.CreateInstance("EAGetMailObj.MailServer");
		outServer->Server = L"pop.gmail.com";
		outServer->User = L"biggie199316@gmail.com";
		outServer->Password = L"";
		//outServer->Protocol = MailServerPop3;

		outServer->SSLConnection = VARIANT_TRUE;
		outServer->Port = 995;

		IMailClientPtr outClient = NULL;
		outClient.CreateInstance("EAGetMailObj.MailClient");
		outClient->LicenseCode = L"TryIt";

		outClient->Connect(outServer);
		wprintf(L"Connected\r\n");

		_variant_t info = outClient->GetMailInfos();
		SAFEARRAY *pSafeArray = info.parray;
		long LBound = 0, UBound = 0;
		SafeArrayGetLBound(pSafeArray, 1, &LBound);
		SafeArrayGetUBound(pSafeArray, 1, &UBound);

		INT count = UBound - LBound + 1;
		wprintf(L"Total %d emails\r\n", count);

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

			outMail->SaveAs(sizeOfFile, VARIANT_TRUE);

			outClient->Delete(pInfo);
		}

		info.Clear();
		outClient->Quit();
	}
	catch (_com_error &errorParameter)
	{
		wprintf(L"Error: %s", (const TCHAR*)errorParameter.Description());
	}
}
