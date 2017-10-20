using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.IO;
using EAGetMail;

namespace GmailOAUTH.csharp
{
    public partial class GmailOAUTHDlg : Form
    {
        // ===============================================================================================================
        // The client_id and client_secret is created for test purposes, please create your clientid
        // 1. Please create your project at https://console.developers.google.com/project;
        // 2. Create a credentials at APIs & Auth -> Credentials -> Add Credentials -> OAuth 2.0 Client ID -> click Other
        // 3. Enable Google+ API in APIs & Auth -> APIs->Google+ API;
        // 4. Finally use your client_id and client_secret to replace the following values;
        // Any problem, please kindly contact support@emailarchitect.net
        // ===============================================================================================================

        private string m_client_id = "1072602369179-aru4rj97ateiho9rt4pf5i8l1r01mc16.apps.googleusercontent.com";
        private string m_client_secret = "Lnw8r5FvfKFNS_CSEucbdIE-";

        private string m_redirect_uri = "urn:ietf:wg:oauth:2.0:oob";
        private string m_scope = "https://www.googleapis.com/auth/plus.profile.emails.read%20https://www.googleapis.com/auth/plus.me%20https://mail.google.com";
        private string m_auth_uri = "https://accounts.google.com/o/oauth2/v2/auth";
        private string m_token_uri = "https://www.googleapis.com/oauth2/v4/token";
        private string m_plus_api_uri = "https://www.googleapis.com/plus/v1/people/me?fields=emails&access_token=";
        public string m_access_token = "";
        public string m_refresh_token = "";
        public string m_user = "";

        public GmailOAUTHDlg()
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;
        }

        // open Google Account Authentication URL
        private void GmailOAUTHDlg_Load(object sender, EventArgs e)
        {

            string authURI = String.Format("{0}?scope={1}&redirect_uri={2}&response_type=code&client_id={3}",
                    m_auth_uri, m_scope, m_redirect_uri, m_client_id);

            webOAUTH.Navigate(authURI); //start to open Gmail OAUTH login web page.
        }

        // Get access code by web browser 
        private void webOAUTH_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            if (m_access_token.Length > 0)
                return;

            System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("code");
            if (elment != null)
            {
                string code = elment.GetAttribute("value");
                GetAccessToken(code);
            }
        }

        // Get access_token from Google server.
        private void GetAccessToken(string code)
        {
            System.Text.Encoding encoder = System.Text.Encoding.GetEncoding("utf-8");
            webOAUTH.Document.Body.InnerHtml = "<div style=\"padding:30px;\" id=\"eas_auth_desc\">Requesting access token from Google server ...</div>";
            Application.DoEvents();

            try
            {
                string url = m_token_uri;
                HttpWebRequest oClient = WebRequest.Create(url) as HttpWebRequest;
                oClient.Method = "POST";
                oClient.ContentType = "application/x-www-form-urlencoded";

                string client_id = m_client_id;
                string client_secret = m_client_secret;
                string redirect_uri = m_redirect_uri;

                string data = "code=" + code;
                data += "&client_id=" + client_id;
                data += "&client_secret=" + client_secret;
                data += "&redirect_uri=" + redirect_uri;
                data += "&grant_type=authorization_code";


                IAsyncResult rqstAr = oClient.BeginGetRequestStream(null, null);
                Application.DoEvents();
                while ((!rqstAr.AsyncWaitHandle.WaitOne(5, false)))
                {
                    Application.DoEvents();
                }
                Stream ofs = oClient.EndGetRequestStream(rqstAr);

                byte[] dt = System.Text.Encoding.UTF8.GetBytes(data);
                ofs.Write(dt, 0, dt.Length);
                ofs.Close();

                IAsyncResult ar = oClient.BeginGetResponse(null, null);
                while ((!ar.AsyncWaitHandle.WaitOne(5, false)))
                {
                    Application.DoEvents();
                }

                HttpWebResponse oResponse = oClient.EndGetResponse(ar) as HttpWebResponse;

                System.IO.StreamReader readStream = new System.IO.StreamReader(oResponse.GetResponseStream(), encoder);
                string responseText = readStream.ReadToEnd();

                m_access_token = SimpleJsonParser.GetJsonValue(responseText, "access_token").Trim("\"\t\r\n ".ToCharArray());
                m_refresh_token = SimpleJsonParser.GetJsonValue(responseText, "refresh_token").Trim("\"\t\r\n ".ToCharArray());

                if ((m_access_token.Length == 0))
                {
                    System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("eas_auth_desc");
                    if ((elment != null))
                    {
                        elment.InnerText = responseText;
                    }
                    return;
                }
            }
            catch (WebException ex)
            {
                string errorDesc = ex.Message;
                if ((ex.Response != null))
                {
                    System.IO.StreamReader readStream = new System.IO.StreamReader(ex.Response.GetResponseStream(), encoder);
                    errorDesc = readStream.ReadToEnd();
                }

                System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("eas_auth_desc");
                if ((elment != null))
                {
                    elment.InnerText = errorDesc;
                }
                return;
            }

            QueryUserProfile(m_access_token);
        }

        // Query authenticated user email address by Google+ API
        private void QueryUserProfile(string access_token)
        {
            string url = m_plus_api_uri;
            url += access_token;

            System.Text.Encoding encoder = System.Text.Encoding.GetEncoding("utf-8");
            try
            {
                System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("eas_auth_desc");
                if ((elment != null))
                {
                    elment.InnerText = "Querying user email address ... ";
                    Application.DoEvents();
                }

                HttpWebRequest oClient = WebRequest.Create(url) as HttpWebRequest;
                oClient.Method = "GET";

                IAsyncResult ar = oClient.BeginGetResponse(null, null);
                while ((!ar.AsyncWaitHandle.WaitOne(5, false)))
                {
                    Application.DoEvents();
                }

                HttpWebResponse oResponse = oClient.EndGetResponse(ar) as HttpWebResponse;
                //   Dim oResponse As HttpWebResponse = oClient.GetResponse()
                System.IO.StreamReader readStream = new System.IO.StreamReader(oResponse.GetResponseStream(), encoder);
                string responseText = readStream.ReadToEnd();
                ParseUserEmail(responseText);

            }
            catch (WebException ex)
            {
                string errorDesc = ex.Message;
                if ((ex.Response != null))
                {
                    System.IO.StreamReader readStream = new System.IO.StreamReader(ex.Response.GetResponseStream(), encoder);
                    errorDesc = readStream.ReadToEnd();
                }

                System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("eas_auth_desc");
                if ((elment != null))
                {
                    elment.InnerText = errorDesc;
                }
                return;
            }
        }

        //Parse result, Query authenticated user email address by Google+ API
        private void ParseUserEmail(string s)
        {
            string emails = SimpleJsonParser.GetJsonValue(s, "emails");
            string[] ar = SimpleJsonParser.ParseArray(emails);

            for (int i = 0; i < ar.Length; i++)
            {
                string t = ar[i];
                string address = SimpleJsonParser.GetJsonValue(t, "value").Trim("\r\n\t \"".ToCharArray());
                string type = SimpleJsonParser.GetJsonValue(t, "type").Trim("\r\n\t \"".ToCharArray());
                if (String.Compare(type, "account", true) != 0)
                {
                    continue;
                }

                m_user = address;
                this.DialogResult = System.Windows.Forms.DialogResult.OK;
                this.Close();
            }

            System.Windows.Forms.HtmlElement elment = webOAUTH.Document.GetElementById("eas_auth_desc");
            if ((elment != null))
            {
                elment.InnerText = "Failed to get user email address!";
            }
        }
    }
}
