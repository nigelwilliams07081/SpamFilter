<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    Friend WithEvents webMail As System.Windows.Forms.WebBrowser

    Friend WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents textServer As System.Windows.Forms.TextBox
    Friend WithEvents chkLeaveCopy As System.Windows.Forms.CheckBox
    Friend WithEvents btnStart As System.Windows.Forms.Button
    Friend WithEvents btnCancel As System.Windows.Forms.Button
    Friend WithEvents lblStatus As System.Windows.Forms.Label
    Friend WithEvents pgBar As System.Windows.Forms.ProgressBar

    Friend WithEvents lstMail As System.Windows.Forms.ListView
    Friend WithEvents colFrom As System.Windows.Forms.ColumnHeader
    Friend WithEvents colSubject As System.Windows.Forms.ColumnHeader
    Friend WithEvents colDate As System.Windows.Forms.ColumnHeader
    Friend WithEvents btnDel As System.Windows.Forms.Button
    Friend WithEvents lblTotal As System.Windows.Forms.Label
    Friend WithEvents label6 As System.Windows.Forms.Label
    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.pgBar = New System.Windows.Forms.ProgressBar
        Me.lblStatus = New System.Windows.Forms.Label
        Me.btnCancel = New System.Windows.Forms.Button
        Me.btnStart = New System.Windows.Forms.Button
        Me.chkLeaveCopy = New System.Windows.Forms.CheckBox
        Me.textServer = New System.Windows.Forms.TextBox
        Me.label1 = New System.Windows.Forms.Label
        Me.lstMail = New System.Windows.Forms.ListView
        Me.colFrom = New System.Windows.Forms.ColumnHeader
        Me.colSubject = New System.Windows.Forms.ColumnHeader
        Me.colDate = New System.Windows.Forms.ColumnHeader
        Me.btnDel = New System.Windows.Forms.Button
        Me.lblTotal = New System.Windows.Forms.Label
        Me.label6 = New System.Windows.Forms.Label
        Me.webMail = New System.Windows.Forms.WebBrowser
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.pgBar)
        Me.groupBox1.Controls.Add(Me.lblStatus)
        Me.groupBox1.Controls.Add(Me.btnCancel)
        Me.groupBox1.Controls.Add(Me.btnStart)
        Me.groupBox1.Controls.Add(Me.chkLeaveCopy)
        Me.groupBox1.Controls.Add(Me.textServer)
        Me.groupBox1.Controls.Add(Me.label1)
        Me.groupBox1.Location = New System.Drawing.Point(8, 8)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(232, 197)
        Me.groupBox1.TabIndex = 0
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = "Account Information"
        '
        'pgBar
        '
        Me.pgBar.Location = New System.Drawing.Point(6, 168)
        Me.pgBar.Name = "pgBar"
        Me.pgBar.Size = New System.Drawing.Size(216, 8)
        Me.pgBar.TabIndex = 15
        '
        'lblStatus
        '
        Me.lblStatus.AutoSize = True
        Me.lblStatus.Location = New System.Drawing.Point(7, 148)
        Me.lblStatus.Name = "lblStatus"
        Me.lblStatus.Size = New System.Drawing.Size(38, 13)
        Me.lblStatus.TabIndex = 14
        Me.lblStatus.Text = "Ready"
        '
        'btnCancel
        '
        Me.btnCancel.Enabled = False
        Me.btnCancel.Location = New System.Drawing.Point(119, 93)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(88, 24)
        Me.btnCancel.TabIndex = 13
        Me.btnCancel.Text = "Cancel"
        '
        'btnStart
        '
        Me.btnStart.Location = New System.Drawing.Point(11, 93)
        Me.btnStart.Name = "btnStart"
        Me.btnStart.Size = New System.Drawing.Size(88, 24)
        Me.btnStart.TabIndex = 12
        Me.btnStart.Text = "Start"
        '
        'chkLeaveCopy
        '
        Me.chkLeaveCopy.Checked = True
        Me.chkLeaveCopy.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkLeaveCopy.Location = New System.Drawing.Point(11, 50)
        Me.chkLeaveCopy.Name = "chkLeaveCopy"
        Me.chkLeaveCopy.Size = New System.Drawing.Size(208, 16)
        Me.chkLeaveCopy.TabIndex = 11
        Me.chkLeaveCopy.Text = "Leave a copy of message on server"
        '
        'textServer
        '
        Me.textServer.Location = New System.Drawing.Point(52, 22)
        Me.textServer.Name = "textServer"
        Me.textServer.Size = New System.Drawing.Size(158, 20)
        Me.textServer.TabIndex = 3
        Me.textServer.Text = "imap.gmail.com"
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(8, 24)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(38, 13)
        Me.label1.TabIndex = 0
        Me.label1.Text = "Server"
        '
        'lstMail
        '
        Me.lstMail.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.colFrom, Me.colSubject, Me.colDate})
        Me.lstMail.FullRowSelect = True
        Me.lstMail.HideSelection = False
        Me.lstMail.Location = New System.Drawing.Point(248, 16)
        Me.lstMail.Name = "lstMail"
        Me.lstMail.Size = New System.Drawing.Size(474, 168)
        Me.lstMail.TabIndex = 1
        Me.lstMail.UseCompatibleStateImageBehavior = False
        Me.lstMail.View = System.Windows.Forms.View.Details
        '
        'colFrom
        '
        Me.colFrom.Text = "From"
        Me.colFrom.Width = 100
        '
        'colSubject
        '
        Me.colSubject.Text = "Subject"
        Me.colSubject.Width = 200
        '
        'colDate
        '
        Me.colDate.Text = "Date"
        Me.colDate.Width = 150
        '
        'btnDel
        '
        Me.btnDel.Location = New System.Drawing.Point(650, 186)
        Me.btnDel.Name = "btnDel"
        Me.btnDel.Size = New System.Drawing.Size(72, 24)
        Me.btnDel.TabIndex = 3
        Me.btnDel.Text = "Delete"
        '
        'lblTotal
        '
        Me.lblTotal.AutoSize = True
        Me.lblTotal.Location = New System.Drawing.Point(256, 192)
        Me.lblTotal.Name = "lblTotal"
        Me.lblTotal.Size = New System.Drawing.Size(0, 13)
        Me.lblTotal.TabIndex = 4
        '
        'label6
        '
        Me.label6.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.label6.Location = New System.Drawing.Point(11, 217)
        Me.label6.Name = "label6"
        Me.label6.Size = New System.Drawing.Size(229, 48)
        Me.label6.TabIndex = 5
        Me.label6.Text = "Warning: if ""leave a copy of message on server"" is not checked,  the emails on th" & _
            "e server will be deleted !"
        '
        'webMail
        '
        Me.webMail.Location = New System.Drawing.Point(248, 214)
        Me.webMail.MinimumSize = New System.Drawing.Size(20, 20)
        Me.webMail.Name = "webMail"
        Me.webMail.Size = New System.Drawing.Size(474, 184)
        Me.webMail.TabIndex = 6
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(734, 412)
        Me.Controls.Add(Me.webMail)
        Me.Controls.Add(Me.label6)
        Me.Controls.Add(Me.lblTotal)
        Me.Controls.Add(Me.btnDel)
        Me.Controls.Add(Me.lstMail)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

End Class
