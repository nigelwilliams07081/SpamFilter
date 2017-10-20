unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, OleCtrls, ComCtrls, StdCtrls, StrUtils, EAGetMailObjLib_TLB,
  SHDocVw;

type
  TForm1 = class(TForm)
    GroupBox1: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    textServer: TEdit;
    textUser: TEdit;
    textPassword: TEdit;
    chkSSL: TCheckBox;
    Label4: TLabel;
    Label5: TLabel;
    lstAuthType: TComboBox;
    lstProtocol: TComboBox;
    chkLeaveCopy: TCheckBox;
    btnStart: TButton;
    btnCancel: TButton;
    lblStatus: TLabel;
    pgBar: TProgressBar;
    Label6: TLabel;
    lstMail: TListView;
    btnDel: TButton;
    lblTotal: TLabel;
    webMail: TWebBrowser;
    procedure FormCreate(Sender: TObject);
    procedure btnStartClick(Sender: TObject);

    // Display email functions
    procedure LoadMails();
    procedure ShowMail( fileName: WideString );
    procedure GenerateHtmlForEmail(htmlName: WideString; emlFile: WideString; tempFolder: WideString);


    // EAGetMail event handler
    procedure OnIdle(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
    procedure OnConnected(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
    procedure OnQuit(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
    procedure OnSecuring(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
    procedure OnAuthorized(ASender: TObject; const oSender: IDispatch;
      var Cancel: WordBool);
    procedure OnSendingDataStream(ASender: TObject; const oSender: IDispatch;
      Sent: Integer; Total: Integer;
      var Cancel: WordBool);
    procedure OnReceivingDataStream(ASender: TObject; const oSender: IDispatch;
      const oInfo: IDispatch;
      Received: Integer; Total: Integer;
      var Cancel: WordBool);
    procedure btnCancelClick(Sender: TObject);
    procedure lstMailSelectItem(Sender: TObject; Item: TListItem;
      Selected: Boolean);
    procedure btnDelClick(Sender: TObject);
    procedure lstMailCompare(Sender: TObject; Item1, Item2: TListItem;
      Data: Integer; var Compare: Integer);
    procedure FormResize(Sender: TObject);
    procedure lstProtocolChange(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;
const
  MailServerConnectType_ConnectSSLAuto = 0;
  MailServerConnectType_ConnectSSL = 1;
  MailServerConnectType_ConnectTLS = 2;

  ProxyType_Socks4 = 0;
  ProxyType_Socks5 = 1;
  ProxyType_Http = 2;

  MailServerAuthLogin = 0;
  MailServerAuthCRAM5 = 1;
  MailServerAuthNTLM = 2;

  MailServerPop3 = 0;
  MailServerImap4 = 1;
  MailServerEWS = 2;   // For Exchange Web Service - Exchange 2007/2010
  MailServerDAV = 3; // For Exchange Web DAV - Exchange 2000/2003

var
  Form1: TForm1;
  m_bCancel: WordBool;
  m_curpath: WideString;
  m_uidlfile: WideString;

implementation

{$R *.dfm}

// EAGetMail event handler
procedure TForm1.OnIdle(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
begin
  Application.ProcessMessages();
  Cancel := m_bCancel;
end;

procedure TForm1.OnConnected(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
begin
  lblStatus.Caption := 'Connected';
  Cancel := m_bCancel;
end;

procedure TForm1.OnQuit(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
begin
  lblStatus.Caption := 'Disconnecting ...';
end;

procedure TForm1.OnSecuring(ASender: TObject; const oSender: IDispatch; var Cancel: WordBool);
begin
  lblStatus.Caption := 'Securing ...';
  Cancel := m_bCancel;
end;

procedure TForm1.OnAuthorized(ASender: TObject; const oSender: IDispatch;
      var Cancel: WordBool);
begin
  lblStatus.Caption := 'Authorized';
  Cancel := m_bCancel;
end;

procedure TForm1.OnSendingDataStream(ASender: TObject; const oSender: IDispatch;
      Sent: Integer; Total: Integer;
      var Cancel: WordBool);
begin
  //implementation code for IMAP4 appending event
end;

procedure TForm1.OnReceivingDataStream(ASender: TObject; const oSender: IDispatch;
      const oInfo: IDispatch;
      Received: Integer; Total: Integer;
      var Cancel: WordBool);
begin
  pgBar.Max := Total;
  pgBar.Min := 0;
  pgBar.Position := Received;
  Cancel := m_bCancel;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  lstAuthType.AddItem('USER/LOGIN', nil);
  lstAuthType.AddItem('APOP/AUTH CRAM-MD5', nil);
  lstAuthType.AddItem('NTLM', nil);
  lstAuthType.ItemIndex := 0;

  lstProtocol.AddItem('POP3', nil);
  lstProtocol.AddItem('IMAP4', nil);
  lstProtocol.AddItem('Exchange Web Service - 2007/2010', nil);
  lstProtocol.AddItem('Exchange WebDAV', nil);
  lstProtocol.ItemIndex := 0;

  webMail.Navigate('about:blank');
  m_curpath := GetCurrentDir();
  m_uidlfile := m_curpath + '\uidl.txt';

  LoadMails();
end;

procedure TForm1.btnStartClick(Sender: TObject);
var
  oMail: IMail;
  oClient: TMailClient;
  oServer: TMailServer;
  oTools: TTools;
  oUIDLManager: TUIDLManager;
  infos: OleVariant;
  i, UBound, Count: integer;
  oInfo: IMailInfo;
  mailFolder: WideString;
  fileName, shortName: WideString;
  item: TListItem;
begin

  if trim(textServer.Text) = '' then
  begin
    ShowMessage( 'Plese input server address!' );
    textServer.SetFocus();
    exit;
  end;

  if(trim(textUser.Text) = '' )  then
  begin
    ShowMessage( 'Please input user!');
    textUser.SetFocus();
    exit;
  end;

  if(trim(textPassword.Text) = '') then
  begin
    ShowMessage( 'Please input password!');
    textPassword.SetFocus();
    exit;
  end;

  oTools := TTools.Create(Application);

  // create a folder named "inbox" under
  // current directory to store the email files
  mailFolder := m_curpath + '\inbox';
  oTools.CreateFolder(mailFolder);

  oClient := TMailClient.Create(Application);
  oClient.LicenseCode := 'TryIt';

  // Add eagetmail event handler
  // You don't have to use EAGetMail Event,
  // but using Event make your application more user friendly
  oClient.OnIdle := OnIdle;
  oClient.OnConnected := OnConnected;
  oClient.OnQuit := OnQuit;
  oClient.OnSecuring := OnSecuring;
  oClient.OnAuthorized := OnAuthorized;
  oClient.OnSendingDataStream := OnSendingDataStream;
  oClient.OnReceivingDataStream := OnReceivingDataStream;
  oUIDLManager := TUIDLManager.Create(Application);

  oServer := TMailServer.Create(Application);
  oServer.Server := trim(textServer.Text);
  oServer.User := trim(textUser.Text);
  oServer.Password := trim(textPassword.Text);
  oServer.AuthType := lstAuthType.ItemIndex;
  oServer.Protocol := lstProtocol.ItemIndex;
  oServer.SSLConnection := chkSSL.Checked;

  if oServer.Protocol = MailServerPop3 then
    if oServer.SSLConnection then
      oServer.Port := 995
    else
      oServer.Port := 110
  else
    if oServer.SSLConnection then
      oServer.Port :=993
    else
      oServer.Port := 143;

  m_bCancel := false;
  lblStatus.Caption := 'Connecting server ...';
  btnStart.Enabled := false;
  btnCancel.Enabled := true;
  pgBar.Position := 0;

  try
   // uidl is the identifier of every email on POP3/IMAP4 server, to avoid retrieve
    // the same email from server more than once, we record the email uidl retrieved every time
    // if you delete the email from server every time and not to leave a copy of email on
    // the server, then please remove all the function about uidl.
    oUIDLManager.Load(m_uidlfile);
     
    oClient.Connect1(oServer.DefaultInterface);


    infos := oClient.GetMailInfos();
    UBound := VarArrayHighBound( infos, 1 );
    Count := UBound+1;

    // Remove uidl that is not existed on server  from local uidl file.
    oUIDLManager.SyncUIDL(oServer.DefaultInterface, infos);
    oUIDLManager.Update(); 

    for i := 0 to UBound do
    begin
      lblStatus.Caption := Format( 'Receiving %d/%d ...', [i+1, Count]);
      oInfo := IDispatch(VarArrayGet(infos, i)) as IMailInfo ;

      if oUIDLManager.FindUIDL(oServer.DefaultInterface, oInfo.UIDL) = nil then
      begin

        // Retrieve email from server
        oMail := oClient.GetMail( oInfo );

        // generate a random file name by current local datetime,
        // you can use your method to generate the filename if you do not like it
        shortName :=  oTools.GenFileName(i) + '.eml';
        fileName := mailFolder + '\' +  shortName;
        oMail.SaveAs( fileName, true );

        item := TListItem.Create(lstMail.Items);
        item.SubItems.Add(oMail.Subject);
        item.SubItems.Add(FormatDateTime('yyyy-MM-dd hh:mm:ss', oMail.ReceivedDate));
        item.SubItems.Add(fileName);
        lstMail.Items.AddItem(item);
        item.Caption := oMail.From.Address;

        

        lblTotal.Caption := Format( 'Total %d email(s)', [lstMail.Items.Count]);

        if chkLeaveCopy.Checked Then
        begin
          //add the email uidl to uidl file to avoid we retrieve it next time.
          oUIDLManager.AddUIDL( oServer.DefaultInterface, oInfo.UIDL, shortName);
        end;
      end;
      pgBar.Position := 0;
    end;

    if chkLeaveCopy.Checked = false then
    begin
      for i := 0 to UBound do
      begin
        lblStatus.Caption := Format( 'Deleting %d ... ', [i+1] );
        oInfo := IDispatch(VarArrayGet(infos, i)) as IMailInfo;
        oClient.Delete( oInfo );
        oUIDLManager.RemoveUIDL(oServer.DefaultInterface, oInfo.UIDL );
      end;
    end;

    // Delete method just mark the email as deleted,
    // Quit method pure the emails from server exactly.
    oClient.Quit;
    lblStatus.Caption := 'Completed';
  except
    on ep:Exception do
    begin
        ShowMessage(ep.Message);
        lblStatus.Caption := ep.Message;
    end;

  end;
  
  oUIDLManager.Update();
  btnStart.Enabled := true;
  btnCancel.Enabled := false;

end;

procedure TForm1.btnCancelClick(Sender: TObject);
begin
  m_bCancel := true;
  btnCancel.Enabled := false;
end;

// Display mails
procedure TForm1.LoadMails();
var
  oMail: TMail;
  oTools: TTools;
  i, UBound: integer;
  find, mailFolder: WideString;
  files: OleVariant;
  fileName: WideString;
  item: TListItem;
begin
  mailFolder := m_curpath + '\inbox';
  find := mailFolder + '\*.eml';

  oTools := TTools.Create(Application);
  oTools.CreateFolder(mailFolder);

  files := oTools.GetFiles(find);
  UBound := VarArrayHighBound( files, 1 );
  for i := 0 to UBound do
  begin
    fileName := VarArrayGet(files, i );
    oMail := TMail.Create(Application);
    oMail.LicenseCode := 'TryIt';

    // Only load the email header temporially
    oMail.LoadFile(fileName, true);

    item := TListItem.Create(lstMail.Items);
    item.SubItems.Add(oMail.Subject);
    item.SubItems.Add(FormatDateTime('yyyy-MM-dd hh:mm:ss', oMail.ReceivedDate));
    item.SubItems.Add(fileName);
    lstMail.Items.AddItem(item);
    item.Caption := oMail.From.Address;
  end;
  lblTotal.Caption := Format( 'Total %d email(s)', [lstMail.Items.Count]);

end;

procedure TForm1.lstMailSelectItem(Sender: TObject; Item: TListItem;
  Selected: Boolean);
var
  fileName: WideString;
begin
  if not Selected then
    exit;
  fileName := item.SubItems[2];

  try
    ShowMail( fileName );
  except
    on ep:Exception do
      ShowMessage( 'Error: ' + ep.Message + #13#10 + 'file: ' + fileName );
  end;
end;

procedure TForm1.ShowMail(fileName: WideString);
var
  mainName, htmlName, tempFolder: WideString;
  oTools: TTools;
begin
// remove .eml extension
  mainName := LeftStr( fileName, length(fileName)-4 );
  htmlName := mainName + '.htm';
  tempFolder := mainName;

  oTools := TTools.Create(Application);
  if not oTools.ExistFile(htmlName) then
  begin
    // we haven't generate the html for this email, generate it now.
    GenerateHtmlForEmail(htmlName, fileName, tempFolder );
  end;

  webMail.Navigate(htmlName);
end;

procedure TForm1.GenerateHtmlForEmail(htmlName: WideString;
emlFile: WideString; tempFolder: WideString);
var
  oMail: TMail;
  oTools: TTools;
  i, UBound: integer;
  att: IAttachment;
  addr: IMailAddress;
  addrs, atts: OleVariant;
  html, hdr, attname: WideString;
begin
  oTools := TTools.Create(Application);
  oMail := TMail.Create(Application);
  oMail.LicenseCode := 'TryIt';

  oMail.LoadFile( emlFile, false );

  try
    if oMail.IsEncrypted then
      oMail.Load(oMail.Decrypt(nil).Content);
  except
    on ep:Exception do
      ShowMessage( 'Decrypt Error: ' + ep.Message );
  end;

  try
    if oMail.IsSigned then
      oMail.VerifySignature();
  except
    on ep:Exception do
      ShowMessage( 'Verify Digital Signature Error: ' + ep.Message );
  end;

  // decode winmail.dat ( TNEF stream ) automatically.
  // also convert RTF body to HTML automatically.
  oMail.DecodeTNEF(); 

  html := oMail.HtmlBody;
  hdr := hdr + '<font face="Courier New,Arial" size="2">';
  hdr := hdr + '<b>From:</b> ' + oMail.From.Name + '&lt;' + oMail.From.Address + '&gt;' + '<br>';

  addrs := oMail.ToAddr;
  UBound := VarArrayHighBound( addrs, 1 );
  if( UBound >= 0) then
  begin
    hdr := hdr + '<b>To:</b>';
    for i:= 0 to UBound do
    begin
      addr := IDispatch(VarArrayGet(addrs, i)) as IMailAddress;
      hdr := hdr + addr.Name + '&lt;' + addr.Address + '&gt';
      if( i < UBound ) then
        hdr := hdr + ';';
    end;
    hdr := hdr + '<br>' + #13#10;
  end;

  addrs := oMail.Cc;
  UBound := VarArrayHighBound( addrs, 1 );
  if( UBound >= 0) then
  begin
    hdr := hdr + '<b>Cc:</b>';
    for i:= 0 to UBound do
    begin
      addr := IDispatch(VarArrayGet(addrs, i)) as IMailAddress;
      hdr := hdr + addr.Name + '&lt;' + addr.Address + '&gt';
      if( i < UBound ) then
        hdr := hdr + ';';
    end;
    hdr := hdr + '<br>' + #13#10;
  end;

  hdr := hdr + '<b>Subject:</b>' + oMail.Subject + '<br>' + #13#10;

  // Parse attachment
  atts := oMail.Attachments;
  UBound := VarArrayHighBound( atts, 1 );
  if( UBound >= 0 ) then
  begin
    // create a temporal folder to store attachments.
    if not oTools.ExistFile(tempFolder) then
      oTools.CreateFolder(tempFolder);

    hdr := hdr + '<b>Attachments:</b>';
    for i:= 0 to UBound do
    begin
      att := IDispatch(VarArrayGet(atts, i)) as IAttachment;

      attname := tempFolder + '\' + att.Name;
      att.SaveAs(attname, true);
      hdr := hdr + '<a href="' + attname + '" target="_blank">' + att.Name + '</a> ';
        // show embedded images
      if att.ContentID <> '' then
      begin
        // StringReplace doesn't support some non-ascii characters very well.
        html := StringReplace( html, 'cid:' + att.ContentID, attname, [rfReplaceAll, rfIgnoreCase]);
      end
      else if Pos('image/', att.ContentType ) = 1 then
      begin
        html := html + '<hr><img src="' + attname + '">';
      end;
    end;
  end;

  hdr := '<meta http-equiv="Content-Type" content="text-html; charset=utf-8">' + hdr;
  html := hdr + '<hr>' + html;
  oTools.WriteTextFile( htmlName, html, 65001 );

end;

procedure TForm1.btnDelClick(Sender: TObject);
var
  index: integer;
  htmlName, mainName, tempFolder, fileName: WideString;
  oTools: TTools;
begin
  if lstMail.ItemIndex = -1 then
  begin
    ShowMessage( 'Please select an email in the list!' );
    exit;
  end;

  oTools := TTools.Create(Application);

  index := lstMail.ItemIndex;
  fileName := lstMail.Items[index].SubItems[2];

  // remove .eml extension
  mainName := LeftStr( fileName, length(fileName)-4 );
  htmlName := mainName + '.htm';
  tempFolder := mainName;

  lstMail.Items.Delete(index);
  try
    oTools.RemoveFile(fileName);
    oTools.RemoveFile(htmlName);
    oTools.RemoveFolder(tempFolder, true);
  except
    on ep:Exception do
    begin
      ShowMessage('Delete Error: ' + ep.Message );
    end;
  end;

  lblTotal.Caption := Format( 'Total %d email(s)', [lstMail.Items.Count]);
  if lstMail.Items.Count > 0 then
  begin
    if index > (lstMail.Items.Count-1) then
        index := lstMail.Items.Count-1;

    lstMail.Items[index].Selected := true;
    ShowMail(lstMail.Items[index].SubItems[2]);
  end
  else
    webMail.Navigate('about:blank');

end;

procedure TForm1.lstMailCompare(Sender: TObject; Item1, Item2: TListItem;
  Data: Integer; var Compare: Integer);
var
  s1, s2: string;
begin
    s1:= Item1.SubItems[1];;
    s2 := Item2.SubItems[1];
    Compare := -CompareText(s1,s2);
end;

procedure TForm1.FormResize(Sender: TObject);
begin
  if Form1.Width < 778 then
    Form1.Width := 778;

  if Form1.Height < 457 then
    Form1.Height := 457;

  lstMail.Width := Form1.Width - 300;
  webMail.Width := lstMail.Width;
  btnDel.Left := Form1.Width - 90;
  webMail.Height := Form1.Height - 250;
end;

procedure TForm1.lstProtocolChange(Sender: TObject);
begin
// by default, Exchange Web Service requires SSL connection
// for other protocols, please set SSL connection based on your server setting.
  if lstProtocol.ItemIndex = MailServerEWS then
    chkSSL.Checked := true;
end;

end.
