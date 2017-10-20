object Form1: TForm1
  Left = 187
  Top = 253
  Width = 786
  Height = 484
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Label6: TLabel
    Left = 8
    Top = 328
    Width = 265
    Height = 49
    AutoSize = False
    Caption = 
      'Warning: if "leave a copy of message on server" is not checked, ' +
      ' the emails on the server will be deleted !'
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    WordWrap = True
  end
  object lblTotal: TLabel
    Left = 280
    Top = 176
    Width = 24
    Height = 13
    Caption = 'Total'
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 265
    Height = 313
    Caption = 'Accout Information'
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 26
      Width = 31
      Height = 13
      Caption = 'Server'
    end
    object lblStatus: TLabel
      Left = 8
      Top = 250
      Width = 31
      Height = 13
      Caption = 'Ready'
    end
    object textServer: TEdit
      Left = 74
      Top = 23
      Width = 177
      Height = 21
      TabOrder = 0
      Text = 'imap.gmail.com'
    end
    object chkLeaveCopy: TCheckBox
      Left = 8
      Top = 64
      Width = 249
      Height = 17
      Caption = 'Leave a copy of message on server'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object btnStart: TButton
      Left = 8
      Top = 136
      Width = 113
      Height = 25
      Caption = 'Start'
      TabOrder = 2
      OnClick = btnStartClick
    end
    object btnCancel: TButton
      Left = 137
      Top = 136
      Width = 113
      Height = 25
      Caption = 'Cancel'
      Enabled = False
      TabOrder = 3
      OnClick = btnCancelClick
    end
    object pgBar: TProgressBar
      Left = 8
      Top = 289
      Width = 243
      Height = 17
      TabOrder = 4
    end
  end
  object lstMail: TListView
    Left = 280
    Top = 13
    Width = 489
    Height = 153
    Columns = <
      item
        Caption = 'From'
        Width = 150
      end
      item
        Caption = 'Subject'
        Width = 190
      end
      item
        Caption = 'Date'
        Width = 150
      end
      item
        Width = 0
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GridLines = True
    HideSelection = False
    RowSelect = True
    ParentFont = False
    SortType = stText
    TabOrder = 1
    ViewStyle = vsReport
    OnCompare = lstMailCompare
    OnSelectItem = lstMailSelectItem
  end
  object btnDel: TButton
    Left = 704
    Top = 171
    Width = 65
    Height = 22
    Caption = 'Delete'
    TabOrder = 2
    OnClick = btnDelClick
  end
  object webMail: TWebBrowser
    Left = 280
    Top = 200
    Width = 489
    Height = 241
    TabOrder = 3
    ControlData = {
      4C0000008A320000E81800000000000000000000000000000000000000000000
      000000004C000000000000000000000001000000E0D057007335CF11AE690800
      2B2E126208000000000000004C0000000114020000000000C000000000000046
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000100000000000000000000000000000000000000}
  end
end
