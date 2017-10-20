//  ===============================================================================
// |    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF      |
// |    ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO    |
// |    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A         |
// |    PARTICULAR PURPOSE.                                                    |
// |    Copyright (c)2016  ADMINSYSTEM SOFTWARE LIMITED                         |
// |
// |    Project: It demonstrates how to use EAGetMail to receive/parse email by Gmail OAUTH.
// |        
// |
// |    File: Form1 : implementation file
// |
// |    Author: Ivan Lui ( ivan@emailarchitect.net )
//  ===============================================================================

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
using System.Collections;
using System.IO;
using EAGetMail;

namespace GmailOAUTH.csharp
{
    public partial class Form1 : Form, IComparer
    {
        private bool m_bcancel = false;
        private string m_uidlfile = "uidl.txt";
        private string m_curpath = "";

        private ArrayList m_arUidl = new ArrayList();
        public Form1()
        {
            InitializeComponent();
        }
        #region EAGetMail Event Handler
        public void OnConnected(object sender, ref bool cancel)
        {
            lblStatus.Text = "Connected ...";
            cancel = m_bcancel;
            Application.DoEvents();
        }

        public void OnQuit(object sender, ref bool cancel)
        {
            lblStatus.Text = "Quit ...";
            cancel = m_bcancel;
            Application.DoEvents();
        }

        public void OnReceivingDataStream(object sender, MailInfo info, int received, int total, ref bool cancel)
        {
            pgBar.Minimum = 0;
            pgBar.Maximum = total;
            pgBar.Value = received;
            cancel = m_bcancel;
            Application.DoEvents();
        }

        public void OnIdle(object sender, ref bool cancel)
        {
            cancel = m_bcancel;
            Application.DoEvents();
        }

        public void OnAuthorized(object sender, ref bool cancel)
        {
            lblStatus.Text = "Authorized ...";
            cancel = m_bcancel;
            Application.DoEvents();
        }

        public void OnSecuring(object sender, ref bool cancel)
        {
            lblStatus.Text = "Securing ...";
            cancel = m_bcancel;
            Application.DoEvents();
        }
        #endregion

        #region Parse and Display Mails
        private void LoadMails()
        {
            lstMail.Items.Clear();
            string mailFolder = String.Format("{0}\\inbox", m_curpath);
            if (!Directory.Exists(mailFolder))
                Directory.CreateDirectory(mailFolder);

            string[] files = Directory.GetFiles(mailFolder, "*.eml");
            int count = files.Length;
            for (int i = 0; i < count; i++)
            {
                string fullname = files[i];
                //For evaluation usage, please use "TryIt" as the license code, otherwise the 
                //"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
                //"trial version expired" exception will be thrown.
                Mail oMail = new Mail("TryIt");

                // Load( file, true ) only load the email header to Mail object to save the CPU and memory
                // the Mail object will load the whole email file later automatically if bodytext or attachment is required..
                oMail.Load(fullname, true);

                ListViewItem item = new ListViewItem(oMail.From.ToString());
                item.SubItems.Add(oMail.Subject);
                item.SubItems.Add(oMail.ReceivedDate.ToString("yyyy-MM-dd HH:mm:ss"));
                item.Tag = fullname;
                lstMail.Items.Add(item);

                int pos = fullname.LastIndexOf(".");
                string mainName = fullname.Substring(0, pos);
                string htmlName = mainName + ".htm";
                if (!File.Exists(htmlName))
                {
                    // this email is unread, we set the font style to bold.
                    item.Font = new System.Drawing.Font(item.Font, FontStyle.Bold);
                }

                oMail.Clear();
            }
        }

        private string _FormatHtmlTag(string src)
        {
            src = src.Replace(">", "&gt;");
            src = src.Replace("<", "&lt;");
            return src;
        }

        //we generate a html + attachment folder for every email, once the html is create,
        // next time we don't need to parse the email again.
        private void _GenerateHtmlForEmail(string htmlName, string emlFile, string tempFolder)
        {
            //For evaluation usage, please use "TryIt" as the license code, otherwise the 
            //"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
            //"trial version expired" exception will be thrown.
            Mail oMail = new Mail("TryIt");
            oMail.Load(emlFile, false);

            if (oMail.IsEncrypted)
            {
                try
                {
                    //this email is encrypted, we decrypt it by user default certificate.
                    // you can also use specified certificate like this
                    // oCert = new Certificate();
                    // oCert.Load("c:\\test.pfx", "pfxpassword", Certificate.CertificateKeyLocation.CRYPT_USER_KEYSET)
                    // oMail = oMail.Decrypt( oCert );
                    oMail = oMail.Decrypt(null);
                }
                catch (Exception ep)
                {
                    MessageBox.Show(ep.Message);
                    oMail.Load(emlFile, false);
                }
            }

            if (oMail.IsSigned)
            {
                try
                {
                    //this email is digital signed.
                    EAGetMail.Certificate cert = oMail.VerifySignature();
                    MessageBox.Show("This email contains a valid digital signature.");
                    //you can add the certificate to your certificate storage like this
                    //cert.AddToStore( Certificate.CertificateStoreLocation.CERT_SYSTEM_STORE_CURRENT_USER,
                    //	"addressbook" );
                    // then you can use send the encrypted email back to this sender.
                }
                catch (Exception ep)
                {
                    MessageBox.Show(ep.Message);
                }
            }

            // decode winmail.dat (Outlook TNEF stream) automatically.
            // also convert RTF body to HTML body automatically.
            oMail.DecodeTNEF();

            string html = oMail.HtmlBody;
            StringBuilder hdr = new StringBuilder();

            hdr.Append("<font face=\"Courier New,Arial\" size=2>");
            hdr.Append("<b>From:</b> " + _FormatHtmlTag(oMail.From.ToString()) + "<br>");
            MailAddress[] addrs = oMail.To;
            int count = addrs.Length;
            if (count > 0)
            {
                hdr.Append("<b>To:</b> ");
                for (int i = 0; i < count; i++)
                {
                    hdr.Append(_FormatHtmlTag(addrs[i].ToString()));
                    if (i < count - 1)
                    {
                        hdr.Append(";");
                    }
                }
                hdr.Append("<br>");
            }

            addrs = oMail.Cc;

            count = addrs.Length;
            if (count > 0)
            {
                hdr.Append("<b>Cc:</b> ");
                for (int i = 0; i < count; i++)
                {
                    hdr.Append(_FormatHtmlTag(addrs[i].ToString()));
                    if (i < count - 1)
                    {
                        hdr.Append(";");
                    }
                }
                hdr.Append("<br>");
            }

            hdr.Append(String.Format("<b>Subject:</b>{0}<br>\r\n", _FormatHtmlTag(oMail.Subject)));

            Attachment[] atts = oMail.Attachments;
            count = atts.Length;
            if (count > 0)
            {
                if (!Directory.Exists(tempFolder))
                    Directory.CreateDirectory(tempFolder);

                hdr.Append("<b>Attachments:</b>");
                for (int i = 0; i < count; i++)
                {
                    Attachment att = atts[i];
                    string attname = String.Format("{0}\\{1}", tempFolder, att.Name);
                    att.SaveAs(attname, true);
                    hdr.Append(String.Format("<a href=\"{0}\" target=\"_blank\">{1}</a> ", attname, att.Name));
                    if (att.ContentID.Length > 0)
                    {	//show embedded image.
                        html = html.Replace("cid:" + att.ContentID, attname);
                    }
                    else if (String.Compare(att.ContentType, 0, "image/", 0, "image/".Length, true) == 0)
                    {
                        //show attached image.
                        html = html + String.Format("<hr><img src=\"{0}\">", attname);
                    }
                }
            }

            Regex reg = new Regex("(<meta[^>]*charset[ \t]*=[ \t\"]*)([^<> \r\n\"]*)", RegexOptions.Multiline | RegexOptions.IgnoreCase);
            html = reg.Replace(html, "$1utf-8");
            if (!reg.IsMatch(html))
            {
                hdr.Insert(0, "<meta HTTP-EQUIV=\"Content-Type\" Content=\"text/html; charset=utf-8\">");
            }

            html = hdr.ToString() + "<hr>" + html;
            FileStream fs = new FileStream(htmlName, FileMode.Create, FileAccess.Write, FileShare.None);
            byte[] data = System.Text.UTF8Encoding.UTF8.GetBytes(html);
            fs.Write(data, 0, data.Length);
            fs.Close();
            oMail.Clear();
        }

        private void ShowMail(string fileName)
        {
            try
            {
                int pos = fileName.LastIndexOf(".");
                string mainName = fileName.Substring(0, pos);
                string htmlName = mainName + ".htm";

                string tempFolder = mainName;
                if (!File.Exists(htmlName))
                {	//we haven't generate the html for this email, generate it now.
                    _GenerateHtmlForEmail(htmlName, fileName, tempFolder);
                }

                object empty = System.Reflection.Missing.Value;
                webMail.Navigate(htmlName);
            }
            catch (Exception ep)
            {
                MessageBox.Show(ep.Message);
            }
        }
        #endregion

        #region IComparer Members
        // sort the email as received data.
        public int Compare(object x, object y)
        {
            ListViewItem itemx = x as ListViewItem;
            ListViewItem itemy = y as ListViewItem;

            string sx = itemx.SubItems[2].Text;
            string sy = itemy.SubItems[2].Text;
            if (sx.Length != sy.Length)
                return -1; //should never occured.

            int count = sx.Length;
            for (int i = 0; i < count; i++)
            {
                if (sx[i] > sy[i])
                    return -1;
                else if (sx[i] < sy[i])
                    return 1;
            }

            return 0;
        }

        #endregion

        private void btnStart_Click(object sender, System.EventArgs e)
        {
            string server;
            server = textServer.Text.Trim();

            if (server.Length == 0)
            {
                MessageBox.Show("Please input server, user and password.");
                return;
            }

            GmailOAUTHDlg dlgOAUTH = new GmailOAUTHDlg();
            if (dlgOAUTH.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string user = dlgOAUTH.m_user;
            string password = dlgOAUTH.m_access_token;

            btnStart.Enabled = false;
            btnCancel.Enabled = true;

            ServerAuthType authType = ServerAuthType.AuthXOAUTH2;

            MailServer oServer = new MailServer(server, user, password,
                true, authType, ServerProtocol.Imap4);

            //For evaluation usage, please use "TryIt" as the license code, otherwise the 
            //"invalid license code" exception will be thrown. However, the object will expire in 1-2 months, then
            //"trial version expired" exception will be thrown.
            MailClient oClient = new MailClient("TryIt");

            //Catching the following events is not necessary, 
            //just make the application more user friendly.
            //If you use the object in asp.net/windows service or non-gui application, 
            //You need not to catch the following events.
            //To learn more detail, please refer to the code in EAGetMail EventHandler region
            oClient.OnAuthorized += new MailClient.OnAuthorizedEventHandler(OnAuthorized);
            oClient.OnConnected += new MailClient.OnConnectedEventHandler(OnConnected);
            oClient.OnIdle += new MailClient.OnIdleEventHandler(OnIdle);
            oClient.OnSecuring += new MailClient.OnSecuringEventHandler(OnSecuring);
            oClient.OnReceivingDataStream += new MailClient.OnReceivingDataStreamEventHandler(OnReceivingDataStream);

            bool bLeaveCopy = chkLeaveCopy.Checked;

            // UIDL is the identifier of every email on POP3/IMAP4/Exchange server, to avoid retrieve
            // the same email from server more than once, we record the email UIDL retrieved every time
            // if you delete the email from server every time and not to leave a copy of email on
            // the server, then please remove all the function about uidl.
            // UIDLManager wraps the function to write/read uidl record from a text file.
            UIDLManager oUIDLManager = new UIDLManager();

            try
            {
                // load existed uidl records to UIDLManager
                string uidlfile = String.Format("{0}\\{1}", m_curpath, m_uidlfile);
                oUIDLManager.Load(uidlfile);

                string mailFolder = String.Format("{0}\\inbox", m_curpath);
                if (!Directory.Exists(mailFolder))
                    Directory.CreateDirectory(mailFolder);

                m_bcancel = false;
                lblStatus.Text = "Connecting ...";
                oClient.Connect(oServer);
                MailInfo[] infos = oClient.GetMailInfos();
                lblStatus.Text = String.Format("Total {0} email(s)", infos.Length);

                // remove the local uidl which is not existed on the server.
                oUIDLManager.SyncUIDL(oServer, infos);
                oUIDLManager.Update();

                int count = infos.Length;

                for (int i = 0; i < count; i++)
                {
                    MailInfo info = infos[i];
                    if (oUIDLManager.FindUIDL(oServer, info.UIDL) != null)
                    {
                        //this email has been downloaded before.
                        continue;
                    }

                    lblStatus.Text = String.Format("Retrieving {0}/{1}...", info.Index, count);

                    Mail oMail = oClient.GetMail(info);
                    System.DateTime d = System.DateTime.Now;
                    System.Globalization.CultureInfo cur = new System.Globalization.CultureInfo("en-US");
                    string sdate = d.ToString("yyyyMMddHHmmss", cur);
                    string fileName = String.Format("{0}\\{1}{2}{3}.eml", mailFolder, sdate, d.Millisecond.ToString("d3"), i);
                    oMail.SaveAs(fileName, true);

                    ListViewItem item = new ListViewItem(oMail.From.ToString());
                    item.SubItems.Add(oMail.Subject);
                    item.SubItems.Add(oMail.ReceivedDate.ToString("yyyy-MM-dd HH:mm:ss"));
                    item.Font = new System.Drawing.Font(item.Font, FontStyle.Bold);
                    item.Tag = fileName;
                    lstMail.Items.Insert(0, item);
                    oMail.Clear();

                    lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count);

                    if (bLeaveCopy)
                    {
                        //add the email uidl to uidl file to avoid we retrieve it next time. 
                        oUIDLManager.AddUIDL(oServer, info.UIDL, fileName);
                    }
                }

                if (!bLeaveCopy)
                {
                    lblStatus.Text = "Deleting ...";
                    for (int i = 0; i < count; i++)
                    {
                        oClient.Delete(infos[i]);
                        // Remove UIDL from local uidl file.
                        oUIDLManager.RemoveUIDL(oServer, infos[i].UIDL);
                    }
                }
                // Delete method just mark the email as deleted, 
                // Quit method pure the emails from server exactly.
                oClient.Quit();

            }
            catch (Exception ep)
            {
                MessageBox.Show(ep.Message);
            }

            // Update the uidl list to local uidl file and then we can load it next time.
            oUIDLManager.Update();

            lblStatus.Text = "Completed";
            pgBar.Maximum = 100;
            pgBar.Minimum = 0;
            pgBar.Value = 0;
            btnStart.Enabled = true;
            btnCancel.Enabled = false;
        }

        private void Form1_Load(object sender, System.EventArgs e)
        {
            object empty = System.Reflection.Missing.Value;
            webMail.Navigate("about:blank");


            string path = Application.ExecutablePath;
            int pos = path.LastIndexOf('\\');
            if (pos != -1)
                path = path.Substring(0, pos);

            m_curpath = path;

            lstMail.Sorting = SortOrder.Descending;
            lstMail.ListViewItemSorter = this;

            LoadMails();
            lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count);
        }

        private void btnCancel_Click(object sender, System.EventArgs e)
        {
            m_bcancel = true;
        }

        private void lstMail_Click(object sender, System.EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = lstMail.SelectedItems;
            if (items.Count == 0)
                return;

            ListViewItem item = items[0] as ListViewItem;
            ShowMail(item.Tag as string);
            item.Font = new System.Drawing.Font(item.Font, FontStyle.Regular);
        }

        private void btnDel_Click(object sender, System.EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = lstMail.SelectedItems;
            if (items.Count == 0)
                return;

            if (MessageBox.Show("Do you want to delete all selected emails",
                            "",
                            MessageBoxButtons.YesNo) == DialogResult.No)
                return;

            while (items.Count > 0)
            {
                try
                {
                    string fileName = items[0].Tag as string;
                    File.Delete(fileName);
                    int pos = fileName.LastIndexOf(".");
                    string tempFolder = fileName.Substring(0, pos);
                    string htmlName = tempFolder + ".htm";
                    if (File.Exists(htmlName))
                        File.Delete(htmlName);

                    if (Directory.Exists(tempFolder))
                    {
                        Directory.Delete(tempFolder, true);
                    }

                    lstMail.Items.Remove(items[0]);
                }
                catch (Exception ep)
                {
                    MessageBox.Show(ep.Message);
                    break;
                }
            }

            lblTotal.Text = String.Format("Total {0} email(s)", lstMail.Items.Count);

            object empty = System.Reflection.Missing.Value;
            webMail.Navigate("about:blank");

        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            if (this.Width < 750)
            {
                this.Width = 750;
            }

            if (this.Height < 450)
            {
                this.Height = 450;
            }

            lstMail.Width = this.Width - 270;
            webMail.Width = lstMail.Width;
            btnDel.Left = this.Width - (btnDel.Width + 20);
            webMail.Height = this.Height - (lstMail.Height + 100);
        }


    }
}
