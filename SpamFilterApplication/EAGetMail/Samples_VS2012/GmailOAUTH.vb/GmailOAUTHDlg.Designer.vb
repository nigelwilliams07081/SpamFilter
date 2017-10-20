<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class GmailOAUTHDlg
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

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.webOAUTH = New System.Windows.Forms.WebBrowser
        Me.SuspendLayout()
        '
        'webOAUTH
        '
        Me.webOAUTH.Dock = System.Windows.Forms.DockStyle.Fill
        Me.webOAUTH.Location = New System.Drawing.Point(0, 0)
        Me.webOAUTH.MinimumSize = New System.Drawing.Size(20, 20)
        Me.webOAUTH.Name = "webOAUTH"
        Me.webOAUTH.Size = New System.Drawing.Size(621, 551)
        Me.webOAUTH.TabIndex = 1
        '
        'GmailOAUTH
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(621, 551)
        Me.Controls.Add(Me.webOAUTH)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "GmailOAUTH"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Gmail OAUTH Example"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents webOAUTH As System.Windows.Forms.WebBrowser
End Class
