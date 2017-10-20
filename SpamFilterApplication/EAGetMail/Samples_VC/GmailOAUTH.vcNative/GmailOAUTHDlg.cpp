// GmailOAUTHDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pop3_imap4_simple.vcNative.h"
#include "GmailOAUTHDlg.h"


// CGmailOAUTHDlg dialog

IMPLEMENT_DYNAMIC(CGmailOAUTHDlg, CDialog)

CGmailOAUTHDlg::CGmailOAUTHDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGmailOAUTHDlg::IDD, pParent)
{
	// ===============================================================================================================
	// The client_id and client_secret is created for test purposes, please create your clientid
	// 1. Please create your project at https://console.developers.google.com/project;
	// 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> click Other
	// 3. Enable Google+ API in APIs & Auth -> APIs->Google+ API;
	// 4. Finally use your client_id and client_secret to replace the following values;
	// Any problem, please kindly contact support@emailarchitect.net
	// ===============================================================================================================

	m_client_id = _T("1072602369179-aru4rj97ateiho9rt4pf5i8l1r01mc16.apps.googleusercontent.com");
	m_client_secret = _T("Lnw8r5FvfKFNS_CSEucbdIE-");

	m_redirect_uri = _T("urn:ietf:wg:oauth:2.0:oob");
	m_scope = _T("https://www.googleapis.com/auth/plus.profile.emails.read%20https://www.googleapis.com/auth/plus.me%20https://mail.google.com");
	m_auth_uri = _T("https://accounts.google.com/o/oauth2/v2/auth");
	m_token_uri = _T("https://www.googleapis.com/oauth2/v4/token");
	m_plus_api_uri = _T("https://www.googleapis.com/plus/v1/people/me?fields=emails&access_token=");
	m_access_token = _T("");
	m_refresh_token = _T("");
	m_user = _T("");
	m_oRequest = NULL;

}

CGmailOAUTHDlg::~CGmailOAUTHDlg()
{
}

void CGmailOAUTHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_OAUTH, webOAUTH);
}


BEGIN_MESSAGE_MAP(CGmailOAUTHDlg, CDialog)
END_MESSAGE_MAP()


// CGmailOAUTHDlg message handlers

BOOL CGmailOAUTHDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString  authURI;
	authURI.Format(_T("%s?scope=%s&redirect_uri=%s&response_type=code&client_id=%s"), 
                    m_auth_uri, m_scope, m_redirect_uri, m_client_id);

	webOAUTH.Navigate( authURI,  NULL, NULL, NULL, NULL );
	webOAUTH.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_EVENTSINK_MAP(CGmailOAUTHDlg, CDialog)
	ON_EVENT(CGmailOAUTHDlg, IDC_EXPLORER_OAUTH, 259, CGmailOAUTHDlg::DocumentCompleteExplorerOauth, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

void CGmailOAUTHDlg::DocumentCompleteExplorerOauth(LPDISPATCH pDisp, VARIANT* URL)
{
	if( m_access_token.GetLength() > 0 )
		return;
	
	CComPtr<IHTMLDocument2> spDoc;
	spDoc = (IHTMLDocument2*)webOAUTH.get_Document();

	CComPtr<IHTMLDocument3> spHtml;
	spDoc->QueryInterface( IID_IHTMLDocument3, (void**)&spHtml );

	CComPtr<IHTMLElement> spElement = NULL;
	spHtml->getElementById( L"code", &spElement );
	if( spElement != NULL )
	{
		CComPtr<IHTMLInputElement> spInput;
		spElement->QueryInterface( IID_IHTMLInputElement, (void**)&spInput );
		if( spInput != NULL )
		{
			BSTR bstr = NULL;
			spInput->get_value( &bstr );
			CString code = bstr;
			::SysFreeString( bstr );
			GetAccessToken( code );
		}
	}
}

void CGmailOAUTHDlg::GetAccessToken( CString &code )
{
	CComPtr<IHTMLDocument2> spDoc;
	spDoc = (IHTMLDocument2*)webOAUTH.get_Document();
	CComPtr<IHTMLElement> spBody;
	spDoc->get_body( &spBody );
	spBody->put_innerHTML( L"<div style=\"padding:30px;\" id=\"eas_auth_desc\">Requesting access token from Google server ...</div>" );
	
	DoEvents();

	CComPtr<IHTMLDocument3> spHtml;
	spDoc->QueryInterface( IID_IHTMLDocument3, (void**)&spHtml );

	CComPtr<IHTMLElement> spElement = NULL;
	spHtml->getElementById( L"eas_auth_desc", &spElement );

	VARIANT vdata;
	::VariantInit( &vdata );

	ISimpleJsonParserPtr oParser = NULL;
	oParser.CreateInstance( __uuidof(EAGetMailObjLib::SimpleJsonParser));

	try
	{
		if( m_oRequest == NULL )
		{
			m_oRequest.CreateInstance("MSXML2.ServerXMLHTTP");
		}
		if( m_oRequest == NULL )
		{
			spElement->put_innerText(L"Failed to create XML HTTP Object, please make sure you install MSXML 3.0 on your machine.\r\n");
			return;
		}

		CString data = _T("code=");
		data += code;
		data += _T("&client_id=");
		data +=  m_client_id;
		data += _T("&client_secret=");
		data +=  m_client_secret;
		data += _T("&redirect_uri=");
		data +=  m_redirect_uri;
		data += _T("&grant_type=authorization_code");
		
		LPSAFEARRAY		psaHunk = NULL;
		LONG			k;

		BYTE cdata = data.GetLength();
		psaHunk = ::SafeArrayCreateVectorEx( VT_UI1, 0, cdata, NULL );
	
		for( k = 0; k < (int)cdata; k++ )
		{
			BYTE ch = (BYTE)data[k];
			::SafeArrayPutElement(psaHunk, &k, &ch );
		}
		
		vdata.vt = ( VT_ARRAY | VT_UI1 );
		vdata.parray = psaHunk;

		_variant_t async(true);
		m_oRequest->setOption((MSXML2::SERVERXMLHTTP_OPTION)2, 13056 );
		_bstr_t url = m_token_uri;

		m_oRequest->open( L"POST", url, async, vtMissing, vtMissing );
		m_oRequest->setRequestHeader( L"Content-Type", L"application/x-www-form-urlencoded");
		m_oRequest->send( vdata );

		while (m_oRequest->readyState != 4) {
			m_oRequest->waitForResponse(1);
			DoEvents();
		}

		long status = 0;
		m_oRequest->get_status(&status);

		if( status < 200 || status >= 300 )
		{
			spElement->put_innerText( L"Failed to get access token from Google server\r\n" );
		}
		else
		{
			BSTR bstrResult = NULL;
			m_oRequest->get_responseText( &bstrResult );
			m_access_token = (LPCTSTR)oParser->Trim(oParser->GetJsonValue( bstrResult, "access_token" ), L"\"\t\r\n " );
			m_refresh_token = (LPCTSTR)oParser->Trim(oParser->GetJsonValue( bstrResult, "refresh_token" ), L"\"\t\r\n " );
		}
	}
	catch( _com_error &ep )
	{
		_bstr_t bstr = ep.Description();
		spElement->put_innerText( L"Failed to get access token from Google server\r\n" );
	}

	::VariantClear( &vdata );

	if( m_access_token.GetLength() > 0 )
	{
		QueryUserProfile( m_access_token );
	}
}
void CGmailOAUTHDlg::QueryUserProfile( CString &access_token )
{
	CString url = m_plus_api_uri;
	url += access_token;

	CComPtr<IHTMLDocument2> spDoc;
	spDoc = (IHTMLDocument2*)webOAUTH.get_Document();

	CComPtr<IHTMLDocument3> spHtml;
	spDoc->QueryInterface( IID_IHTMLDocument3, (void**)&spHtml );

	CComPtr<IHTMLElement> spElement = NULL;
	spHtml->getElementById( L"eas_auth_desc", &spElement );

	try
	{
		spElement->put_innerText( L"Querying user email address ... " );
		DoEvents();

		if( m_oRequest == NULL )
		{
			m_oRequest.CreateInstance("MSXML2.ServerXMLHTTP");
		}
		if( m_oRequest == NULL )
		{
			spElement->put_innerText(L"Failed to create XML HTTP Object, please make sure you install MSXML 3.0 on your machine.\r\n");
			return;
		}
		_variant_t async(true);
		m_oRequest->setOption((MSXML2::SERVERXMLHTTP_OPTION)2, 13056 );

		m_oRequest->open( L"GET", (LPCTSTR)url, async, vtMissing, vtMissing );
		m_oRequest->send( vtMissing );

		while (m_oRequest->readyState != 4) {
			m_oRequest->waitForResponse(1);
			DoEvents();
		}

		long status = 0;
		m_oRequest->get_status(&status);

		if( status < 200 || status >= 300 )
		{
			spElement->put_innerText( L"Failed to get user profile from Google server\r\n" );
		}
		else
		{
			BSTR bstrResult = NULL;
			m_oRequest->get_responseText( &bstrResult );
			CString s = bstrResult;
			::SysFreeString( bstrResult );
			ParseUserEmail( s );
		}

	}
	catch (_com_error &ep)
	{
		_bstr_t bstr = ep.Description();
		spElement->put_innerText( L"Failed to get access token from Google server\r\n" );
	}
}

void CGmailOAUTHDlg::ParseUserEmail( CString &s )
{
	ISimpleJsonParserPtr oParser = NULL;
	
	oParser.CreateInstance( __uuidof(EAGetMailObjLib::SimpleJsonParser));
	CString emails = oParser->GetJsonValue((LPCTSTR)s, _T("emails"));
    ISimpleJsonArrayPtr ar = oParser->ParseArray((LPCTSTR)emails);
	int count = ar->Length;
	for (int i = 0; i < count; i++)
	{
		CString t = ar->Item(i);
		CString address = oParser->GetJsonValue((LPCTSTR)t, _T("value"));
		address = (LPCTSTR)oParser->Trim((LPCTSTR)address, _T("\r\n\t \""));

		CString type = oParser->GetJsonValue((LPCTSTR)t, _T("type"));
		type = (LPCTSTR)oParser->Trim((LPCTSTR)type, "\r\n\t \"" );
		if (type.CompareNoCase( _T("account")) != 0)
		{
			continue;
		}

		m_user = address;
		CDialog::OnOK();
	}
}

void CGmailOAUTHDlg::DoEvents()
{
	MSG msg; 
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{ 
		if(msg.message == WM_QUIT) 
			return; 

		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}  
}