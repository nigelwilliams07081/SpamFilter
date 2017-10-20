Imports System.Windows.Forms
Imports System.Net
Imports System.IO
Imports EAGetMail

Public Class GmailOAUTHDlg
    ' ===============================================================================================================
    ' The client_id and client_secret is created for test purposes, please create your clientid
    ' 1. Please create your project at https://console.developers.google.com/project;
    ' 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> click Other
    ' 3. Enable Google+ API in APIs & Auth -> APIs->Google+ API;
    ' 4. Finally use your client_id and client_secret to replace the following values;
    ' Any problem, please kindly contact support@emailarchitect.net
    ' ===============================================================================================================

    Private m_clientid As String = "1072602369179-aru4rj97ateiho9rt4pf5i8l1r01mc16.apps.googleusercontent.com"
    Private m_client_secret As String = "Lnw8r5FvfKFNS_CSEucbdIE-"

    Private m_redirect_uri As String = "urn:ietf:wg:oauth:2.0:oob"
    Private m_scope As String = "https://www.googleapis.com/auth/plus.profile.emails.read%20https://www.googleapis.com/auth/plus.me%20https://mail.google.com" '%20
    Private m_auth_uri As String = "https://accounts.google.com/o/oauth2/v2/auth"
    Private m_token_uri As String = "https://www.googleapis.com/oauth2/v4/token"

    Public m_access_token As String = ""
    Public m_refresh_token As String = ""
    Public m_user As String = ""


    Private Sub GmailOAUTH_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim authURI As String
        authURI = String.Format("{0}?scope={1}&redirect_uri={2}&response_type=code&client_id={3}", _
             m_auth_uri, m_scope, m_redirect_uri, m_clientid)

        webOAUTH.Navigate(authURI) ' start to open Gmail OAUTH login web page.

    End Sub

    Private Sub webOAUTH_DocumentCompleted(ByVal sender As System.Object, ByVal e As System.Windows.Forms.WebBrowserDocumentCompletedEventArgs) Handles webOAUTH.DocumentCompleted
        Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("code")
        If m_access_token.Length > 0 Then
            Exit Sub
        End If

        If Not (elment Is Nothing) Then
            Dim code As String = elment.GetAttribute("value")
            GetAccessToken(code)
        End If
    End Sub

    Private Sub GetAccessToken(ByVal code As String)

        Dim encoder As System.Text.Encoding = System.Text.Encoding.GetEncoding("utf-8")

        webOAUTH.Document.Body.InnerHtml = "<div style=""padding:30px;"" id=""eas_auth_desc"">Requesting access token from Google server ...</div>"
        Application.DoEvents()

        Try
            Dim url As String = m_token_uri
            Dim oClient As HttpWebRequest = WebRequest.Create(url)
            oClient.Method = "POST"
            oClient.ContentType = "application/x-www-form-urlencoded"

            Dim client_id As String = m_clientid
            Dim client_secret As String = m_client_secret
            Dim redirect_uri As String = m_redirect_uri

            Dim data As String = "code=" + code
            data += "&client_id=" + client_id
            data += "&client_secret=" + client_secret
            data += "&redirect_uri=" + redirect_uri
            data += "&grant_type=authorization_code"


            Dim rqstAr As IAsyncResult = oClient.BeginGetRequestStream(Nothing, Nothing)
            Application.DoEvents()
            While (Not rqstAr.AsyncWaitHandle.WaitOne(5, False))
                Application.DoEvents()
            End While
            Dim ofs As Stream = oClient.EndGetRequestStream(rqstAr)

            Dim dt As Byte() = System.Text.Encoding.UTF8.GetBytes(data)
            ofs.Write(dt, 0, dt.Length)
            ofs.Close()

            Dim ar As IAsyncResult = oClient.BeginGetResponse(Nothing, Nothing)
            While (Not ar.AsyncWaitHandle.WaitOne(5, False))
                Application.DoEvents()
            End While

            Dim oResponse As HttpWebResponse = oClient.EndGetResponse(ar)

            Dim readStream As New System.IO.StreamReader(oResponse.GetResponseStream(), encoder)
            Dim responseText As String = readStream.ReadToEnd()

            m_access_token = SimpleJsonParser.GetJsonValue(responseText, "access_token").Trim((""" " & Chr(9) & Chr(13) & Chr(10)).ToCharArray())
            m_refresh_token = SimpleJsonParser.GetJsonValue(responseText, "refresh_token").Trim((""" " & Chr(9) & Chr(13) & Chr(10)).ToCharArray())

            If (m_access_token.Length = 0) Then
                Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("eas_auth_desc")
                If Not (elment Is Nothing) Then
                    elment.InnerText = responseText
                End If
                Exit Sub
            End If
        Catch ex As WebException
            Dim errorDesc As String = ex.Message
            If Not (ex.Response Is Nothing) Then
                Dim readStream As New System.IO.StreamReader(ex.Response.GetResponseStream(), encoder)
                errorDesc = readStream.ReadToEnd()
            End If

            Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("eas_auth_desc")
            If Not (elment Is Nothing) Then
                elment.InnerText = errorDesc
            End If
            Exit Sub
        End Try

        QueryUserProfile(m_access_token)
    End Sub

    Private Sub QueryUserProfile(ByVal access_token As String)
        Dim url As String = "https://www.googleapis.com/plus/v1/people/me?fields=emails&access_token="
        url += access_token

        Dim encoder As System.Text.Encoding = System.Text.Encoding.GetEncoding("utf-8")
        Try
            Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("eas_auth_desc")
            If Not (elment Is Nothing) Then
                elment.InnerText = "Querying user email address ... "
                Application.DoEvents()
            End If

            Dim oClient As HttpWebRequest = WebRequest.Create(url)
            oClient.Method = "GET"

            Dim ar As IAsyncResult = oClient.BeginGetResponse(Nothing, Nothing)
            While (Not ar.AsyncWaitHandle.WaitOne(5, False))
                Application.DoEvents()
            End While

            Dim oResponse As HttpWebResponse = oClient.EndGetResponse(ar)
            '   Dim oResponse As HttpWebResponse = oClient.GetResponse()
            Dim readStream As New System.IO.StreamReader(oResponse.GetResponseStream(), encoder)
            Dim responseText As String = readStream.ReadToEnd()
            ParseUserEmail(responseText)

        Catch ex As WebException
            Dim errorDesc As String = ex.Message
            If Not (ex.Response Is Nothing) Then
                Dim readStream As New System.IO.StreamReader(ex.Response.GetResponseStream(), encoder)
                errorDesc = readStream.ReadToEnd()
            End If

            Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("eas_auth_desc")
            If Not (elment Is Nothing) Then
                elment.InnerText = errorDesc
            End If
            Exit Sub

        End Try
    End Sub

    Private Sub ParseUserEmail(ByVal s As String)
        Dim emails As String = SimpleJsonParser.GetJsonValue(s, "emails").Trim(("[] " & Chr(9) & Chr(10) & Chr(13)).ToCharArray())
        Dim email As String = SimpleJsonParser.GetJsonValue(emails, "value")
        email = email.Trim((""" " & Chr(9) & Chr(10) & Chr(13)).ToCharArray())
        m_user = email

        If (m_user.Length = 0) Then
            Dim elment As System.Windows.Forms.HtmlElement = webOAUTH.Document.GetElementById("eas_auth_desc")
            If Not (elment Is Nothing) Then
                elment.InnerText = "Failed to get user email address!"
            End If
            Exit Sub
        End If

        Me.DialogResult = Windows.Forms.DialogResult.OK
        Me.Close()

    End Sub
End Class
