namespace GmailOAUTH.csharp
{
    partial class GmailOAUTHDlg
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.webOAUTH = new System.Windows.Forms.WebBrowser();
            this.SuspendLayout();
            // 
            // webOAUTH
            // 
            this.webOAUTH.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webOAUTH.Location = new System.Drawing.Point(0, 0);
            this.webOAUTH.MinimumSize = new System.Drawing.Size(20, 20);
            this.webOAUTH.Name = "webOAUTH";
            this.webOAUTH.Size = new System.Drawing.Size(615, 556);
            this.webOAUTH.TabIndex = 0;
            this.webOAUTH.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.webOAUTH_DocumentCompleted);
            // 
            // GmailOAUTHDlg
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(615, 556);
            this.Controls.Add(this.webOAUTH);
            this.Name = "GmailOAUTHDlg";
            this.Text = "GmailOAUTHDlg";
            this.Load += new System.EventHandler(this.GmailOAUTHDlg_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.WebBrowser webOAUTH;
    }
}