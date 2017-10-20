object Form2: TForm2
  Left = 192
  Top = 107
  BorderStyle = bsDialog
  Caption = 'Gmail OAUTH'
  ClientHeight = 552
  ClientWidth = 572
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object webOAUTH: TWebBrowser
    Left = 0
    Top = 8
    Width = 569
    Height = 529
    TabOrder = 0
    OnDocumentComplete = webOAUTHDocumentComplete
    ControlData = {
      4C000000CF3A0000AC3600000000000000000000000000000000000000000000
      000000004C000000000000000000000001000000E0D057007335CF11AE690800
      2B2E126208000000000000004C0000000114020000000000C000000000000046
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000100000000000000000000000000000000000000}
  end
end
