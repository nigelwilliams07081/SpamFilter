namespace GmailOAUTH.csharp
{
    partial class Form1
    {
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textServer;
        private System.Windows.Forms.CheckBox chkLeaveCopy;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.ProgressBar pgBar;

        private System.Windows.Forms.ListView lstMail;
        private System.Windows.Forms.ColumnHeader colFrom;
        private System.Windows.Forms.ColumnHeader colSubject;
        private System.Windows.Forms.ColumnHeader colDate;
        private System.Windows.Forms.Button btnDel;
        private System.Windows.Forms.Label lblTotal;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.WebBrowser webMail;
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.pgBar = new System.Windows.Forms.ProgressBar();
            this.lblStatus = new System.Windows.Forms.Label();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.chkLeaveCopy = new System.Windows.Forms.CheckBox();
            this.textServer = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lstMail = new System.Windows.Forms.ListView();
            this.colFrom = new System.Windows.Forms.ColumnHeader();
            this.colSubject = new System.Windows.Forms.ColumnHeader();
            this.colDate = new System.Windows.Forms.ColumnHeader();
            this.btnDel = new System.Windows.Forms.Button();
            this.lblTotal = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.webMail = new System.Windows.Forms.WebBrowser();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.pgBar);
            this.groupBox1.Controls.Add(this.lblStatus);
            this.groupBox1.Controls.Add(this.btnCancel);
            this.groupBox1.Controls.Add(this.btnStart);
            this.groupBox1.Controls.Add(this.chkLeaveCopy);
            this.groupBox1.Controls.Add(this.textServer);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(8, 8);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(232, 202);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Account Information";
            // 
            // pgBar
            // 
            this.pgBar.Location = new System.Drawing.Point(9, 165);
            this.pgBar.Name = "pgBar";
            this.pgBar.Size = new System.Drawing.Size(216, 8);
            this.pgBar.TabIndex = 15;
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(8, 140);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(38, 13);
            this.lblStatus.TabIndex = 14;
            this.lblStatus.Text = "Ready";
            // 
            // btnCancel
            // 
            this.btnCancel.Enabled = false;
            this.btnCancel.Location = new System.Drawing.Point(128, 83);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(88, 24);
            this.btnCancel.TabIndex = 13;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(11, 83);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(88, 24);
            this.btnStart.TabIndex = 12;
            this.btnStart.Text = "Start";
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // chkLeaveCopy
            // 
            this.chkLeaveCopy.Checked = true;
            this.chkLeaveCopy.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLeaveCopy.Location = new System.Drawing.Point(11, 48);
            this.chkLeaveCopy.Name = "chkLeaveCopy";
            this.chkLeaveCopy.Size = new System.Drawing.Size(208, 16);
            this.chkLeaveCopy.TabIndex = 11;
            this.chkLeaveCopy.Text = "Leave a copy of message on server";
            // 
            // textServer
            // 
            this.textServer.Location = new System.Drawing.Point(80, 22);
            this.textServer.Name = "textServer";
            this.textServer.Size = new System.Drawing.Size(136, 20);
            this.textServer.TabIndex = 3;
            this.textServer.Text = "imap.gmail.com";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Server";
            // 
            // lstMail
            // 
            this.lstMail.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colFrom,
            this.colSubject,
            this.colDate});
            this.lstMail.FullRowSelect = true;
            this.lstMail.HideSelection = false;
            this.lstMail.Location = new System.Drawing.Point(248, 16);
            this.lstMail.Name = "lstMail";
            this.lstMail.Size = new System.Drawing.Size(474, 168);
            this.lstMail.TabIndex = 1;
            this.lstMail.UseCompatibleStateImageBehavior = false;
            this.lstMail.View = System.Windows.Forms.View.Details;
            this.lstMail.Click += new System.EventHandler(this.lstMail_Click);
            // 
            // colFrom
            // 
            this.colFrom.Text = "From";
            this.colFrom.Width = 100;
            // 
            // colSubject
            // 
            this.colSubject.Text = "Subject";
            this.colSubject.Width = 200;
            // 
            // colDate
            // 
            this.colDate.Text = "Date";
            this.colDate.Width = 150;
            // 
            // btnDel
            // 
            this.btnDel.Location = new System.Drawing.Point(650, 186);
            this.btnDel.Name = "btnDel";
            this.btnDel.Size = new System.Drawing.Size(72, 24);
            this.btnDel.TabIndex = 3;
            this.btnDel.Text = "Delete";
            this.btnDel.Click += new System.EventHandler(this.btnDel_Click);
            // 
            // lblTotal
            // 
            this.lblTotal.AutoSize = true;
            this.lblTotal.Location = new System.Drawing.Point(256, 192);
            this.lblTotal.Name = "lblTotal";
            this.lblTotal.Size = new System.Drawing.Size(0, 13);
            this.lblTotal.TabIndex = 4;
            // 
            // label6
            // 
            this.label6.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.label6.Location = new System.Drawing.Point(8, 228);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(232, 48);
            this.label6.TabIndex = 5;
            this.label6.Text = "Warning: if \"leave a copy of message on server\" is not checked,  the emails on th" +
                "e server will be deleted !";
            // 
            // webMail
            // 
            this.webMail.Location = new System.Drawing.Point(248, 216);
            this.webMail.MinimumSize = new System.Drawing.Size(20, 20);
            this.webMail.Name = "webMail";
            this.webMail.Size = new System.Drawing.Size(474, 184);
            this.webMail.TabIndex = 6;
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(734, 412);
            this.Controls.Add(this.webMail);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.lblTotal);
            this.Controls.Add(this.btnDel);
            this.Controls.Add(this.lstMail);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        #endregion
    }
}

