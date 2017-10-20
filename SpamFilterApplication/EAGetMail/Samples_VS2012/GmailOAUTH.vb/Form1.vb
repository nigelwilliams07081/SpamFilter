'  ===============================================================================
' |    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF      |
' |    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO    |
' |    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A         |
' |    PARTICULAR PURPOSE.                                                    |
' |    Copyright (c)2016  ADMINSYSTEM SOFTWARE LIMITED                         |
' |
' |    Project: It demonstrates how to use EAGetMail to receive/parse email using Gmail OAUTH.
' |        
' |
' |    File: Form1 : implementation file
' |
' |    Author: Ivan Lui ( ivan@emailarchitect.net )
'  ===============================================================================
Imports System.IO
Imports System.Text
Imports System.Text.RegularExpressions
Imports EAGetMail

Public Class Form1
    Inherits System.Windows.Forms.Form
    Implements IComparer

    Private m_arUidl As ArrayList = New ArrayList
    Private m_bcancel As Boolean = False
    Private m_uidlfile As String = "uidl.txt"
    Private m_curpath As String = ""

#Region "EAGetMail Event Handler"
    Private Sub OnConnected(ByVal sender As Object, ByRef cancel As Boolean)
        lblStatus.Text = "Connected ..."
        cancel = m_bcancel
        Application.DoEvents()
    End Sub

    Private Sub OnQuit(ByVal sender As Object, ByRef cancel As Boolean)
        lblStatus.Text = "Quit ..."
        cancel = m_bcancel
        Application.DoEvents()
    End Sub

    Private Sub OnReceivingDataStream(ByVal sender As Object, ByVal info As MailInfo, ByVal received As Integer, ByVal total As Integer, ByRef cancel As Boolean)
        pgBar.Minimum = 0
        pgBar.Maximum = total
        pgBar.Value = received
        cancel = m_bcancel
        Application.DoEvents()
    End Sub

    Private Sub OnIdle(ByVal sender As Object, ByRef cancel As Boolean)
        cancel = m_bcancel
        Application.DoEvents()
    End Sub

    Private Sub OnAuthorized(ByVal sender As Object, ByRef cancel As Boolean)
        lblStatus.Text = "Authorized ..."
        cancel = m_bcancel
        Application.DoEvents()
    End Sub

    Private Sub OnSecuring(ByVal sender As Object, ByRef cancel As Boolean)
        lblStatus.Text = "Securing ..."
        cancel = m_bcancel
        Application.DoEvents()
    End Sub
#End Region

#Region "Parse and Display Mails"
    Private Sub LoadMails()
        lstMail.Items.Clear()
        Dim mailFolder As String = String.Format("{0}\inbox", m_curpath)

        If (Not Directory.Exists(mailFolder)) Then
            Directory.CreateDirectory(mailFolder)
        End If

        Dim files() As String = Directory.GetFiles(mailFolder, "*.eml")
        Dim count As Integer = files.Length
        For i As Integer = 0 To count - 1
            Dim fullname As String = files(i)
            'For evaluation usage, please use "TryIt" as the license code, otherwise the 
            '"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
            '"trial version expired" exception will be thrown.
            Dim oMail As New Mail("TryIt")

            'Load( file, true ) only load the email header to Mail object to save the CPU and memory
            ' the Mail object will load the whole email file later automatically if bodytext or attachment is required..
            oMail.Load(fullname, True)

            Dim item As ListViewItem = New ListViewItem(oMail.From.ToString())
            item.SubItems.Add(oMail.Subject)
            item.SubItems.Add(oMail.ReceivedDate.ToString("yyyy-MM-dd HH:mm:ss"))
            item.Tag = fullname
            lstMail.Items.Add(item)

            Dim pos As Integer = fullname.LastIndexOf(".")
            Dim mainName As String = fullname.Substring(0, pos)
            Dim htmlName As String = mainName + ".htm"
            If Not File.Exists(htmlName) Then
                ' this email is unread, we set the font style to bold.
                item.Font = New System.Drawing.Font(item.Font, FontStyle.Bold)
            End If
            oMail.Clear()
        Next
    End Sub

    Private Function _FormatHtmlTag(ByVal src As String) As String
        src = src.Replace(">", "&gt;")
        src = src.Replace("<", "&lt;")
        _FormatHtmlTag = src
    End Function

    'we generate a html + attachment folder for every email, once the html is create,
    ' next time we don't need to parse the email again.
    Private Sub _GenerateHtmlForEmail(ByVal htmlName As String, ByVal emlFile As String, ByVal tempFolder As String)
        'For evaluation usage, please use "TryIt" as the license code, otherwise the 
        '"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
        '"trial version expired" exception will be thrown.
        Dim oMail As New Mail("TryIt")
        oMail.Load(emlFile, False)

        If (oMail.IsEncrypted) Then
            Try
                'this email is encrypted, we decrypt it by user default certificate.
                ' you can also use specified certificate like this
                ' oCert = new Certificate()
                'oCert.Load("c:\test.pfx", "pfxpassword", Certificate.CertificateKeyLocation.CRYPT_USER_KEYSET)
                ' oMail = oMail.Decrypt( oCert )
                oMail = oMail.Decrypt(Nothing)
            Catch ep As Exception
                MessageBox.Show(ep.Message)
                oMail.Load(emlFile, False)
            End Try
        End If

        If (oMail.IsSigned) Then
            Try
                'this email is digital signed.
                Dim cert As EAGetMail.Certificate = oMail.VerifySignature()
                MessageBox.Show("This email contains a valid digital signature.")
                'you can add the certificate to your certificate storage like this
                'cert.AddToStore( Certificate.CertificateStoreLocation.CERT_SYSTEM_STORE_CURRENT_USER,"addressbook" );
                ' then you can use send the encrypted email back to this sender.
            Catch ep As Exception
                MessageBox.Show(ep.Message)
            End Try
        End If

        ' decode winmail.dat (Outlook TNEF Stream) automatically.
        ' also convert RTF body to HTML body automatically
        oMail.DecodeTNEF()

        Dim html As String = oMail.HtmlBody
        Dim hdr As New StringBuilder
        hdr.Append("<font face=""Courier New,Arial"" size=2>")
        hdr.Append("<b>From:</b> " + _FormatHtmlTag(oMail.From.ToString()) + "<br>")

        Dim addrs() As MailAddress = oMail.To
        Dim count As Integer = addrs.Length
        If (count > 0) Then
            hdr.Append("<b>To:</b> ")
            For i As Integer = 0 To count - 1
                hdr.Append(_FormatHtmlTag(addrs(i).ToString()))
                If (i < count - 1) Then
                    hdr.Append(";")
                End If
            Next
            hdr.Append("<br>")
        End If

        addrs = oMail.Cc
        count = addrs.Length
        If (count > 0) Then
            hdr.Append("<b>Cc:</b> ")
            For i As Integer = 0 To count - 1

                hdr.Append(_FormatHtmlTag(addrs(i).ToString()))
                If (i < count - 1) Then
                    hdr.Append(";")
                End If
            Next
            hdr.Append("<br>")
        End If

        hdr.Append(String.Format("<b>Subject:</b>{0}<br>" & vbCrLf, _FormatHtmlTag(oMail.Subject)))

        Dim atts() As Attachment = oMail.Attachments
        count = atts.Length
        If (count > 0) Then

            If (Not Directory.Exists(tempFolder)) Then
                Directory.CreateDirectory(tempFolder)
            End If

            hdr.Append("<b>Attachments:</b>")
            For i As Integer = 0 To count - 1
                Dim att As Attachment = atts(i)
                'this attachment is in OUTLOOK RTF format, decode it here.

                Dim attname As String = String.Format("{0}\{1}", tempFolder, att.Name)
                att.SaveAs(attname, True)
                hdr.Append(String.Format("<a href=""{0}"" target=""_blank"">{1}</a> ", attname, att.Name))
                If (att.ContentID.Length > 0) Then
                    'show embedded image.
                    html = html.Replace("cid:" + att.ContentID, attname)
                ElseIf (String.Compare(att.ContentType, 0, "image/", 0, "image/".Length, True) = 0) Then
                    'show attached image.
                    html = html + String.Format("<hr><img src=""{0}"">", attname)
                End If
            Next
        End If

        Dim reg As Regex = New Regex("(<meta[^>]*charset[ \t]*=[ \t""]*)([^<> \r\n""]*)", RegexOptions.Multiline Or RegexOptions.IgnoreCase)
        html = reg.Replace(html, "$1utf-8")
        If Not (reg.IsMatch(html)) Then
            hdr.Insert(0, "<meta HTTP-EQUIV=""Content-Type"" Content=""text/html; charset=utf-8"">")
        End If

        html = hdr.ToString() + "<hr>" + html
        Dim fs As New FileStream(htmlName, FileMode.Create, FileAccess.Write, FileShare.None)
        Dim data() As Byte = System.Text.UTF8Encoding.UTF8.GetBytes(html)
        fs.Write(data, 0, data.Length)
        fs.Close()
        oMail.Clear()
    End Sub

    Private Sub ShowMail(ByVal fileName As String)

        Try
            Dim pos As Integer = fileName.LastIndexOf(".")
            Dim mainName As String = fileName.Substring(0, pos)
            Dim htmlName As String = mainName + ".htm"

            Dim tempFolder As String = mainName
            If Not (File.Exists(htmlName)) Then
                'we haven't generate the html for this email, generate it now.
                _GenerateHtmlForEmail(htmlName, fileName, tempFolder)
            End If
            webMail.Navigate(htmlName)
        Catch ep As Exception
            MessageBox.Show(ep.Message)
        End Try
    End Sub
#End Region

#Region "IComparer Members"
    'sort the email as received data.
    Function Compare(ByVal x As Object, ByVal y As Object) As Integer Implements IComparer.Compare
        Dim itemx As ListViewItem = x
        Dim itemy As ListViewItem = y

        Dim sx() As Char = itemx.SubItems(2).Text.ToCharArray()
        Dim sy() As Char = itemy.SubItems(2).Text.ToCharArray()
        If sx.Length <> sy.Length Then
            Compare = -1
            Exit Function 'should never occured.
        End If

        Dim count As Integer = sx.Length
        For i As Integer = 0 To count - 1

            If sx(i) > sy(i) Then
                Compare = -1
                Exit Function
            ElseIf sx(i) < sy(i) Then
                Compare = 1
                Exit Function
            End If
        Next
        Compare = 0
    End Function

#End Region

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        webMail.Navigate("about:blank")

        Dim path As String = Application.ExecutablePath
        Dim pos As Integer = path.LastIndexOf("\")
        If pos <> -1 Then
            path = path.Substring(0, pos)
        End If
        m_curpath = path

        lstMail.Sorting = SortOrder.Descending
        lstMail.ListViewItemSorter = Me

        LoadMails()
        lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count)
    End Sub

    Private Sub btnCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCancel.Click
        m_bcancel = True
    End Sub

    Private Sub btnDel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDel.Click
        Dim items As ListView.SelectedListViewItemCollection = lstMail.SelectedItems
        If items.Count = 0 Then
            Exit Sub
        End If

        If MessageBox.Show("Do you want to delete all selected emails", _
                                 "", _
                                 MessageBoxButtons.YesNo) = DialogResult.No Then
            Exit Sub
        End If

        Do While (items.Count > 0)
            Try
                Dim fileName As String = items(0).Tag
                File.Delete(fileName)
                Dim pos As Integer = fileName.LastIndexOf(".")
                Dim tempFolder As String = fileName.Substring(0, pos)
                Dim htmlName As String = tempFolder + ".htm"
                If (File.Exists(htmlName)) Then
                    File.Delete(htmlName)
                End If

                If (Directory.Exists(tempFolder)) Then
                    Directory.Delete(tempFolder, True)
                End If
                lstMail.Items.Remove(items(0))
            Catch ep As Exception
                MessageBox.Show(ep.Message)
                Exit Do
            End Try
        Loop

        lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count)
        webMail.Navigate("about:blank")
    End Sub

    Private Sub btnStart_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStart.Click

        'open Gmail OAUTH Dialog
        Dim dlgAuth As New GmailOAUTHDlg()

        If dlgAuth.ShowDialog() <> Windows.Forms.DialogResult.OK Then
            Return
        End If

        Dim server, user, password As String
        server = textServer.Text.Trim()
        user = dlgAuth.m_user
        password = dlgAuth.m_access_token

        If (server.Length = 0) Then
            MessageBox.Show("Please input server, user and password.")
            Exit Sub
        End If

        btnStart.Enabled = False
        btnCancel.Enabled = True

        Dim authType As ServerAuthType = ServerAuthType.AuthXOAUTH2
        Dim protocol As ServerProtocol = ServerProtocol.Imap4

        Dim oServer As MailServer = New MailServer(server, user, password, _
        True, authType, protocol)

        'For evaluation usage, please use "TryIt" as the license code, otherwise the 
        '"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
        '"trial version expired" exception will be thrown.
        Dim oClient As MailClient = New MailClient("TryIt")

        'Catching the following events is not necessary, 
        'just make the application more user friendly.
        'If you use the object in asp.net/windows service or non-gui application, 
        'You need not to catch the following events.
        'To learn more detail, please refer to the code in EAGetMail EventHandler region

        AddHandler oClient.OnAuthorized, AddressOf OnAuthorized
        AddHandler oClient.OnConnected, AddressOf OnConnected
        AddHandler oClient.OnIdle, AddressOf OnIdle
        AddHandler oClient.OnSecuring, AddressOf OnSecuring
        AddHandler oClient.OnReceivingDataStream, AddressOf OnReceivingDataStream
        Dim bLeaveCopy As Boolean = chkLeaveCopy.Checked

        ' UIDL is the identifier of every email on POP3/IMAP4/Exchange server, to avoid retrieve
        ' the same email from server more than once, we record the email UIDL retrieved every time
        ' if you delete the email from server every time and not to leave a copy of email on
        ' the server, then please remove all the function about uidl.
        ' UIDLManager wraps the function to write/read uidl record from a text file.
        Dim oUIDLManager As New UIDLManager()

        Try

            ' load existed uidl records to UIDLManager
            Dim uidlfile As String = String.Format("{0}\{1}", m_curpath, m_uidlfile)
            oUIDLManager.Load(uidlfile)


            Dim mailFolder As String = String.Format("{0}\inbox", m_curpath)
            If Not Directory.Exists(mailFolder) Then
                Directory.CreateDirectory(mailFolder)
            End If

            m_bcancel = False
            lblStatus.Text = "Connecting ..."
            oClient.Connect(oServer)
            Dim infos() As MailInfo = oClient.GetMailInfos()
            lblStatus.Text = String.Format("Total {0} email(s)", infos.Length)

            ' remove the local uidl which is not existed on the server.
            oUIDLManager.SyncUIDL(oServer, infos)
            oUIDLManager.Update()

            Dim count As Integer = infos.Length

            For i As Integer = 0 To count - 1
                Dim info As MailInfo = infos(i)
                If oUIDLManager.FindUIDL(oServer, info.UIDL) Is Nothing Then
                    ' This email has not been downloaded, download it now.

                    lblStatus.Text = String.Format("Retrieving {0}/{1}...", info.Index, count)
                    Dim oMail As Mail = oClient.GetMail(info)
                    Dim d As System.DateTime = System.DateTime.Now
                    Dim cur As System.Globalization.CultureInfo = New System.Globalization.CultureInfo("en-US")
                    Dim sdate As String = d.ToString("yyyyMMddHHmmss", cur)
                    Dim fileName As String = String.Format("{0}\{1}{2}{3}.eml", mailFolder, sdate, d.Millisecond.ToString("d3"), i)
                    oMail.SaveAs(fileName, True)

                    Dim item As ListViewItem = New ListViewItem(oMail.From.ToString())
                    item.SubItems.Add(oMail.Subject)
                    item.SubItems.Add(oMail.ReceivedDate.ToString("yyyy-MM-dd HH:mm:ss"))
                    item.Font = New System.Drawing.Font(item.Font, FontStyle.Bold)
                    item.Tag = fileName
                    lstMail.Items.Insert(0, item)
                    oMail.Clear()
                    lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count)

                    If (bLeaveCopy) Then
                        ' Add the email UIDL to uidl file to avoid we retrieve it next time. 
                        oUIDLManager.AddUIDL(oServer, info.UIDL, fileName)
                    End If
                End If
            Next

            If Not (bLeaveCopy) Then
                lblStatus.Text = "Deleting ..."
                For i As Integer = 0 To count - 1
                    oClient.Delete(infos(i))
                    ' Remove UIDL from local uidl file.
                    oUIDLManager.RemoveUIDL(oServer, infos(i).UIDL)
                Next
            End If
            ' Delete method just mark the email as deleted, 
            ' Quit method pure the emails from server exactly.
            oClient.Quit()

        Catch ep As Exception
            MessageBox.Show(ep.Message)
        End Try

        ' Update the uidl list to local uidl text file and then we can load it next time.
        oUIDLManager.Update()

        lblStatus.Text = "Completed"
        pgBar.Maximum = 100
        pgBar.Minimum = 0
        pgBar.Value = 0
        btnStart.Enabled = True
        btnCancel.Enabled = False
    End Sub

    Private Sub lstMail_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstMail.Click
        Dim items As ListView.SelectedListViewItemCollection = lstMail.SelectedItems
        If items.Count = 0 Then
            Exit Sub
        End If
        Dim item As ListViewItem = items(0)
        ShowMail(item.Tag)
        item.Font = New System.Drawing.Font(item.Font, FontStyle.Regular)
    End Sub

    Private Sub Form1_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Resize
        If Me.Width < 750 Then
            Me.Width = 750
        End If

        If Me.Height < 450 Then
            Me.Height = 450
        End If

        lstMail.Width = Me.Width - 270
        webMail.Width = lstMail.Width
        btnDel.Left = Me.Width - (btnDel.Width + 20)
        webMail.Height = Me.Height - (lstMail.Height + 100)
    End Sub
End Class
