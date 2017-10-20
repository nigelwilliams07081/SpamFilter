VERSION 5.00
Object = "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}#1.1#0"; "shdocvw.dll"
Begin VB.Form DlgOAUTH 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Gmail OAUTH"
   ClientHeight    =   8520
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   7710
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   8520
   ScaleWidth      =   7710
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin SHDocVwCtl.WebBrowser webOAUTH 
      Height          =   8055
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   7455
      ExtentX         =   13150
      ExtentY         =   14208
      ViewMode        =   0
      Offline         =   0
      Silent          =   0
      RegisterAsBrowser=   0
      RegisterAsDropTarget=   1
      AutoArrange     =   0   'False
      NoClientEdge    =   0   'False
      AlignLeft       =   0   'False
      NoWebView       =   0   'False
      HideFileNames   =   0   'False
      SingleClick     =   0   'False
      SingleSelection =   0   'False
      NoFolders       =   0   'False
      Transparent     =   0   'False
      ViewID          =   "{0057D0E0-3573-11CF-AE69-08002B2E1262}"
      Location        =   "http:///"
   End
End
Attribute VB_Name = "DlgOAUTH"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ===============================================================================
' |    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF      |
' |    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO    |
' |    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A         |
' |    PARTICULAR PURPOSE.                                                    |
' |    Copyright (c)2015 ADMINSYSTEM SOFTWARE LIMITED                         |
' |
' |
' |
' The client_id and client_secret is created for test purposes, please create your clientid
' 1. Please create your project at https://console.developers.google.com/project;
' 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> Other
' 3 Enable Google+ API in APIs & Auth -> APIs->Google+ API;
' 4. Finally use your client_id and client_secret to replace the following values;
' Any problem, please kindly contact support@emailarchitect.net
' =================================================================================*/
Option Explicit

Public m_ok As Boolean

Const m_client_id = "1072602369179-aru4rj97ateiho9rt4pf5i8l1r01mc16.apps.googleusercontent.com"
Const m_client_secret = "Lnw8r5FvfKFNS_CSEucbdIE-"

Const m_redirect_uri = "urn:ietf:wg:oauth:2.0:oob"
Const m_scope = "https://www.googleapis.com/auth/plus.profile.emails.read%20https://www.googleapis.com/auth/plus.me%20https://mail.google.com"
Const m_auth_uri = "https://accounts.google.com/o/oauth2/v2/auth"
Const m_token_uri = "https://www.googleapis.com/oauth2/v4/token"
Const m_plus_api_uri = "https://www.googleapis.com/plus/v1/people/me?fields=emails&access_token="

Public m_access_token As String
Public m_refresh_token As String
Public m_user As String

Private m_oRequest As MSXML2.ServerXMLHTTP

Private Sub Form_Load()
    m_ok = False
    
    Dim authURI As String
    authURI = m_auth_uri & "?scope=" & m_scope & "&redirect_uri=" & m_redirect_uri & "&response_type=code&client_id=" & m_client_id
        
    webOAUTH.Navigate authURI
End Sub

Private Sub webOAUTH_DocumentComplete(ByVal pDisp As Object, URL As Variant)
    If m_access_token <> "" Then
        Exit Sub
    End If
    
    Dim htmlDoc As HTMLDocument
    Set htmlDoc = webOAUTH.Document
    
    
    Dim htmlInput As HTMLInputElement
    Set htmlInput = htmlDoc.getElementById("code")
    
    If htmlInput Is Nothing Then
        Exit Sub
    End If
    
    Dim code As String
    code = htmlInput.Value
    
    GetAccessToken code
    
End Sub

Sub GetAccessToken(code)
    
    Dim htmlDoc As HTMLDocument
    Set htmlDoc = webOAUTH.Document
    
    Dim htmlBody As htmlBody
    Set htmlBody = htmlDoc.body
    
    htmlBody.innerHTML = "<div style=""padding:30px;"" id=""eas_auth_desc"">Requesting access token from Google server ...</div>"
    DoEvents
    
    Dim htmlDesc As HTMLHtmlElement
    Set htmlDesc = htmlDoc.getElementById("eas_auth_desc")
    
    Set m_oRequest = New MSXML2.ServerXMLHTTP
    
    Dim data
    data = "code=" & code
    data = data & "&client_id=" & m_client_id
    data = data & "&client_secret=" & m_client_secret
    data = data & "&redirect_uri=" & m_redirect_uri
    data = data & "&grant_type=authorization_code"

    m_oRequest.setOption 2, 13056
    m_oRequest.open "POST", m_token_uri, True
    m_oRequest.setRequestHeader "Content-Type", "application/x-www-form-urlencoded"
    m_oRequest.Send data
    
    Do While m_oRequest.ReadyState <> 4
        DoEvents
        m_oRequest.waitForResponse (1)
    Loop
    
    Dim status
    status = m_oRequest.status
    
    If status < 200 Or status >= 300 Then
        htmlDesc.innerText = "Failed to get access token from Google server."
        Exit Sub
    End If

    Dim result
    result = m_oRequest.responseText

    Dim oParser As New EAGetMailObjLib.SimpleJsonParser
    m_access_token = oParser.Trim(oParser.GetJsonValue(result, "access_token"), """ " & Chr(9) & Chr(10) & Chr(13))
    m_refresh_token = oParser.Trim(oParser.GetJsonValue(result, "refresh_token"), """ " & Chr(9) & Chr(10) & Chr(13))

    QueryUserProfile m_access_token

End Sub

Sub QueryUserProfile(access_token)
    If m_oRequest Is Nothing Then
        Set m_oRequest = New MSXML2.ServerXMLHTTP
    End If
    
    Dim htmlDoc As HTMLDocument
    Set htmlDoc = webOAUTH.Document
    
    Dim htmlDesc As HTMLHtmlElement
    Set htmlDesc = htmlDoc.getElementById("eas_auth_desc")
     
    htmlDesc.innerText = "Query user profile from Google server ..."
    DoEvents
    
    m_oRequest.setOption 2, 13056
    m_oRequest.open "GET", m_plus_api_uri & access_token, True
    m_oRequest.Send

    Do While m_oRequest.ReadyState <> 4
        DoEvents
        m_oRequest.waitForResponse (1)
    Loop
    
    Dim status
    status = m_oRequest.status
    
    If status < 200 Or status >= 300 Then
        htmlDesc.innerText = "Failed to get user profile from Google server."
        Exit Sub
    End If
    
    Dim result
    result = m_oRequest.responseText

    ParseUserEmail (result)
End Sub

Sub ParseUserEmail(s)
    
    Dim htmlDoc As HTMLDocument
    Set htmlDoc = webOAUTH.Document
    
    Dim htmlDesc As HTMLHtmlElement
    Set htmlDesc = htmlDoc.getElementById("eas_auth_desc")
     
     
    Dim oParser As New EAGetMailObjLib.SimpleJsonParser
    
    Dim emails
    emails = oParser.GetJsonValue(s, "emails")
    
    Dim ar
    Set ar = oParser.ParseArray(emails)

    Dim i, count
    count = ar.Length
    For i = 0 To count - 1
        Dim t, address, addr_type
        t = ar.Item(i)
        address = oParser.GetJsonValue(t, "value")
        address = oParser.Trim(address, """ " & Chr(9) & Chr(10) & Chr(13))
    
        addr_type = oParser.GetJsonValue(t, "type")
        addr_type = oParser.Trim(addr_type, """ " & Chr(9) & Chr(10) & Chr(13))

        If LCase(addr_type) = "account" Then
            m_user = address
            m_ok = True
            Unload Me
            Exit Sub
        End If
        
    Next

    htmlDesc.innerText = "Failed to get user profile (email) from Google server."
    
End Sub
