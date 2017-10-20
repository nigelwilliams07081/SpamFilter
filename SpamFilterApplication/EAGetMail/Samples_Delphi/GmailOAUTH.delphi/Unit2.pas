unit Unit2;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, EAGetMailObjLib_TLB, OleCtrls, SHDocVw, MSHTML, MSXML2_TLB;

type
  TForm2 = class(TForm)
    webOAUTH: TWebBrowser;
    procedure FormCreate(Sender: TObject);
    procedure webOAUTHDocumentComplete(Sender: TObject;
      const pDisp: IDispatch; var URL: OleVariant);
    procedure FormShow(Sender: TObject);
    procedure GetAccessToken( code: string );
    procedure QueryUserProfile( access_token: string );
    procedure ParseUserEmail( s: string );
  private
    { Private declarations }
    m_client_id: string;
    m_client_secret: string;
    m_redirect_uri: string;
    m_scope: string;
    m_auth_uri: string;
    m_token_uri: string;
    m_plus_api_uri: string;
    m_oRequest: TServerXMLHTTP;
    m_code: string;

  public
    { Public declarations }
    m_user: string;
    m_access_token: string;
    m_refresh_token: string;

  end;

var
  Form2: TForm2;

  

implementation

{$R *.dfm}

procedure TForm2.FormCreate(Sender: TObject);

begin
// ===============================================================================================================
	// The client_id and client_secret is created for test purposes, please create your clientid
	// 1. Please create your project at https://console.developers.google.com/project;
	// 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> click Other
	// 3. Enable Google+ API in APIs & Auth -> APIs->Google+ API;
	// 4. Finally use your client_id and client_secret to replace the following values;
	// Any problem, please kindly contact support@emailarchitect.net
	// ===============================================================================================================

	m_client_id := '1072602369179-aru4rj97ateiho9rt4pf5i8l1r01mc16.apps.googleusercontent.com';
	m_client_secret := 'Lnw8r5FvfKFNS_CSEucbdIE-';

	m_redirect_uri := 'urn:ietf:wg:oauth:2.0:oob';
	m_scope := 'https://www.googleapis.com/auth/plus.profile.emails.read%20https://www.googleapis.com/auth/plus.me%20https://mail.google.com';
	m_auth_uri := 'https://accounts.google.com/o/oauth2/v2/auth';
	m_token_uri := 'https://www.googleapis.com/oauth2/v4/token';
	m_plus_api_uri := 'https://www.googleapis.com/plus/v1/people/me?fields=emails&access_token=';
	m_access_token := '';
	m_refresh_token := '';
	m_user := '';
  m_code := '';
end;

procedure TForm2.webOAUTHDocumentComplete(Sender: TObject;
  const pDisp: IDispatch; var URL: OleVariant);
var
  htmlDoc: HTMLDocument;
  htmlInput: IHtmlInputElement;
  code: string;
begin

  if m_code <> '' then
    exit;

  if m_access_token <> '' then
    exit;

  htmlDoc := webOAUTH.Document as HTMLDocument;
  htmlInput := htmlDoc.getElementById('code') as IHtmlInputElement;
  if htmlInput = nil then
    exit;

  code := htmlInput.value;
  //ShowMessage( code );
  m_code := code;
  GetAccessToken( code );

end;

procedure TForm2.FormShow(Sender: TObject);
var
  authURI: string;
begin
  authURI := m_auth_uri + '?scope=' + m_scope + '&redirect_uri=' + m_redirect_uri;
  authURI := authURI + '&response_type=code&client_id=' + m_client_id;
  m_code := '';
  webOAUTH.Navigate( authURI );

  //ShowMessage( authURI );
end;

procedure TForm2.GetAccessToken( code: string );
var
  htmlDoc: HTMLDocument;
  htmlBody: HTMLHtmlElement;
  htmlDesc: HTMLHtmlElement;
  data: OleVariant;
  status: integer;
  result, t: string;
  oParser: TSimpleJsonParser;
begin
  htmlDoc := webOAUTH.Document as HTMLDocument;
  htmlBody := htmlDoc.body as HTMLHtmlElement;
  
  htmlBody.innerHTML := '<div style="padding:30px;" id="eas_auth_desc">Requesting access token from Google server...</div>';
  Application.ProcessMessages();

  htmlDesc := htmlDoc.getElementById('eas_auth_desc') as HTMLHtmlElement;


  if m_oRequest = nil then
  begin
    m_oRequest := TServerXMLHTTP.Create(Application);
  end;

  if m_oRequest = nil then
  begin
    htmlDesc.innerText := 'Failed to create XML HTTP Object, please make sure you installed MS XML 3.0 on your machine';
    exit;
  end;

  try
    data := 'code=';
    data := data + code;
    data := data + '&client_id=';
    data := data + m_client_id;
    data := data + '&client_secret=';
    data := data + m_client_secret;
    data := data + '&redirect_uri=';
    data := data + m_redirect_uri;
    data := data + '&grant_type=authorization_code';

    m_oRequest.setOption(2, 13056);
    m_oRequest.open('POST', m_token_uri, true);
    m_oRequest.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    m_oRequest.send(data);

    while( m_oRequest.readyState <> 4 ) do
    begin
      m_oRequest.waitForResponse(1);
      Application.ProcessMessages();
    end;

    status := m_oRequest.status;
    if (status < 200) or (status >= 300) then
    begin
      htmlDesc.innerText := 'Failed to get access token from Google server.';
      exit;
    end;

    result := m_oRequest.responseText;
    oParser := TSimpleJsonParser.Create(Application);
    t := oParser.GetJsonValue(result, 'access_token');
    m_access_token := oParser.Trim(t, #13+#10+#9+' "');

    t := oParser.GetJsonValue(result, 'refresh_token');
    m_refresh_token := oParser.Trim(t, #13+#10+#9+' "');


  except

    htmlDesc.innerText := 'Failed to get access token!';
    exit;
  end;

  QueryUserProfile( m_access_token );

end;

procedure TForm2.QueryUserProfile( access_token: string );
var
  htmlDoc: HTMLDocument;
  htmlDesc: HTMLHtmlElement;
  url, result: string;
  status: integer;
begin
  htmlDoc := webOAUTH.Document as HTMLDocument;
  htmlDesc := htmlDoc.getElementById('eas_auth_desc') as HTMLHtmlElement;

  url := m_plus_api_uri;
  url := url + access_token;

  htmlDesc.innerText := 'Querying user email address ... ';
  Application.ProcessMessages();

  try
    m_oRequest.setOption(2, 13056);
    m_oRequest.open('GET', url, true );
    m_oRequest.send();

    while( m_oRequest.readyState <> 4 ) do
    begin
      m_oRequest.waitForResponse(1);
      Application.ProcessMessages();
    end;

    status := m_oRequest.status;
    if (status < 200) or (status >= 300) then
    begin
      htmlDesc.innerText := 'Failed to get user profile from Google server.';
      exit;
    end;
    result := m_oRequest.responseText;

    ParseUserEmail(result);
  except
    htmlDesc.innerText := 'Failed to get user profile from Google server.';
    exit;
  end;
end;

procedure TForm2.ParseUserEmail( s: string );
var
  oParser: TSimpleJsonParser;
  ar: ISimpleJsonArray;
  emails, t, address, addrtype, tt: string;
  i, count: integer;
begin
    
   oParser := TSimpleJsonParser.Create(Application);
   emails := oParser.GetJsonValue(s, 'emails');
   ar := oParser.ParseArray( emails );
   count := ar.Count;
   for i := 0 to count - 1 do
   begin

    t := ar.Item(i);
    
    tt := oParser.GetJsonValue(t, 'value');
    address := oParser.Trim( tt, #13+#10+#9+' "' );
    tt := oParser.GetJsonValue(t, 'type');
    addrtype := oParser.Trim( tt, #13+#10+#9+' "' );

    if CompareText( addrtype, 'account' ) = 0 then
    begin
      m_user := address;  
      ModalResult := mrOK;
    end;

   end;
end;

end.
 