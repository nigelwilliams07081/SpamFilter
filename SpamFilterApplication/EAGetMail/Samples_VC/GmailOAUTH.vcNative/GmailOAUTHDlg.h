#pragma once
#include "explorer_mail.h"


// CGmailOAUTHDlg dialog

class CGmailOAUTHDlg : public CDialog
{
	DECLARE_DYNAMIC(CGmailOAUTHDlg)

public:
	CGmailOAUTHDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGmailOAUTHDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OAUTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	void GetAccessToken( CString &code );
	void QueryUserProfile( CString &access_token );
	void ParseUserEmail( CString &s );
	void DoEvents();
	// ===============================================================================================================
	// The client_id and client_secret is created for test purposes, please create your clientid
	// 1. Please create your project at https://console.developers.google.com/project;
	// 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> click Other
	// 3. Enable Google+ API in APIs & Auth -> APIs->Google+ API;
	// 4. Finally use your client_id and client_secret to replace the following values;
	// Any problem, please kindly contact support@emailarchitect.net
	// ===============================================================================================================
private:
	IServerXMLHTTPRequestPtr m_oRequest;
	CString m_client_id;
	CString m_client_secret;

	CString m_redirect_uri;
	CString m_scope;
	CString m_auth_uri;
	CString m_token_uri;
	CString m_plus_api_uri;

public:

	CString m_access_token;
	CString m_refresh_token;
	CString m_user;
	CExplorer_mail webOAUTH;
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorerOauth(LPDISPATCH pDisp, VARIANT* URL);
};

