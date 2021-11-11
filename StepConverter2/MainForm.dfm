object MForm: TMForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'MForm'
  ClientHeight = 639
  ClientWidth = 864
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PrintScale = poNone
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object SpeedButton4: TSpeedButton
    Tag = 3
    Left = 904
    Top = 312
    Width = 89
    Height = 22
    Caption = 'ALL_CLIENTS'
    OnClick = OptionsClick
  end
  object SpeedButton5: TSpeedButton
    Tag = 4
    Left = 904
    Top = 340
    Width = 89
    Height = 22
    Caption = 'ToSERVER'
    OnClick = OptionsClick
  end
  object Panel1: TPanel
    Left = 4
    Top = 331
    Width = 376
    Height = 290
    Caption = 'Panel1'
    Color = clRed
    ParentBackground = False
    TabOrder = 6
  end
  object VLDSImageDisplay1: TVLDSImageDisplay
    Left = 8
    Top = 335
    Width = 369
    Height = 282
    Proportional = False
    InputPin.Form = MForm
    InputPin.SourcePin = AVG.FilterTransform_2.OutputPin
    Graph.AdditionalFilters = <>
    UseVMR = True
  end
  object SLScope1: TSLScope
    Left = 383
    Top = 335
    Width = 429
    Height = 286
    Cursor = crDefault
    Title.Align = vaTop
    Title.Visible = False
    Title.Text = 'Scope'
    Title.Font.Charset = DEFAULT_CHARSET
    Title.Font.Color = clWhite
    Title.Font.Height = -21
    Title.Font.Name = 'Arial'
    Title.Font.Style = [fsBold]
    Title.AdditionalTitles = <>
    ToolBar.Buttons.ZoomOff.Visible = False
    ToolBar.Buttons.ZoomPrevious.Visible = False
    ToolBar.Buttons.ZoomNext.Visible = False
    ToolBar.Buttons.ZoomIn.Visible = False
    ToolBar.Buttons.ZoomOut.Visible = False
    ToolBar.Buttons.ZoomMode.Visible = False
    ToolBar.Buttons.XYZoomOn.Visible = False
    ToolBar.Buttons.Copy.Visible = False
    ToolBar.Buttons.Print.Visible = False
    ToolBar.Buttons.Save.Visible = False
    Trails.Font.Charset = DEFAULT_CHARSET
    Trails.Font.Color = clWhite
    Trails.Font.Height = -11
    Trails.Font.Name = 'Arial'
    Trails.Font.Style = []
    RefreshInterval = 500
    TabOrder = 0
    InputPins.Form = MForm
    InputPins.Pins = (
      (
        AVG.HistogramMain.OutputPins._Pin0
        'AVG.HistogramMain.OutputPins.Red')
      (
        AVG.HistogramMain.OutputPins._Pin1
        'AVG.HistogramMain.OutputPins.Green')
      (
        AVG.HistogramMain.OutputPins._Pin2
        'AVG.HistogramMain.OutputPins.Blue'))
    XInputPins.Form = MForm
    YAxis.Align = vaLeft
    YAxis.MajorTicks.Step = 10.000000000000000000
    YAxis.MajorTicks.StartFrom._Floats = (
      (
        StartFrom
        0.000000000000000000))
    YAxis.Format.Precision = 3
    YAxis.Font.Charset = DEFAULT_CHARSET
    YAxis.Font.Color = clWhite
    YAxis.Font.Height = -11
    YAxis.Font.Name = 'Arial'
    YAxis.Font.Style = []
    YAxis.Min.Range.High.Value = 1000.000000000000000000
    YAxis.Min.Range.High.Enabled = False
    YAxis.Min.Range.Low.Enabled = False
    YAxis.Min.Range.Low._Floats = (
      (
        Value
        0.000000000000000000))
    YAxis.Min.Tick._Floats = (
      (
        Value
        0.000000000000000000))
    YAxis.Min._Floats = (
      (
        DataValue
        0.000000000000000000)
      (
        Value
        0.000000000000000000))
    YAxis.Max.Value = 1000.000000000000000000
    YAxis.Max.DataValue = 1000.000000000000000000
    YAxis.Max.Range.High.Value = 1000.000000000000000000
    YAxis.Max.Range.High.Enabled = False
    YAxis.Max.Range.Low.Value = -1000.000000000000000000
    YAxis.Max.Range.Low.Enabled = False
    YAxis.Max.Tick._Floats = (
      (
        Value
        0.000000000000000000))
    YAxis.Zooming.Range.High.Value = 100000000.000000000000000000
    YAxis.Zooming.Range.High.Enabled = False
    YAxis.Zooming.Range.Low.Value = 0.000000100000000000
    YAxis.Zooming.Range.Low.Enabled = True
    YAxis.AxisLabel.Font.Charset = DEFAULT_CHARSET
    YAxis.AxisLabel.Font.Color = clWhite
    YAxis.AxisLabel.Font.Height = -13
    YAxis.AxisLabel.Font.Name = 'Arial'
    YAxis.AxisLabel.Font.Style = [fsBold]
    YAxis.AxisLabel.Visible = False
    YAxis.AxisLabel.Text = 'Y Axis'
    YAxis.DataView.Lines.Pen.Color = clGreen
    YAxis.DataView.ZeroLine.Pen.Color = clWhite
    YAxis.AdditionalAxes = <>
    XAxis.Align = vaBottom
    XAxis.MajorTicks.Step = 10.000000000000000000
    XAxis.MajorTicks.StartFrom._Floats = (
      (
        StartFrom
        0.000000000000000000))
    XAxis.Format.Precision = 3
    XAxis.Font.Charset = DEFAULT_CHARSET
    XAxis.Font.Color = clWhite
    XAxis.Font.Height = -11
    XAxis.Font.Name = 'Arial'
    XAxis.Font.Style = []
    XAxis.Min.AutoScale = False
    XAxis.Min.Range.High.Value = 1000.000000000000000000
    XAxis.Min.Range.High.Enabled = False
    XAxis.Min.Range.Low.Value = -1000.000000000000000000
    XAxis.Min.Range.Low.Enabled = False
    XAxis.Min.Tick._Floats = (
      (
        Value
        0.000000000000000000))
    XAxis.Min._Floats = (
      (
        DataValue
        0.000000000000000000)
      (
        Value
        0.000000000000000000))
    XAxis.Max.AutoScale = False
    XAxis.Max.Value = 256.000000000000000000
    XAxis.Max.DataValue = 256.000000000000000000
    XAxis.Max.Range.High.Value = 1000.000000000000000000
    XAxis.Max.Range.High.Enabled = False
    XAxis.Max.Range.Low.Value = -1000.000000000000000000
    XAxis.Max.Range.Low.Enabled = False
    XAxis.Max.Tick.Value = 1024.000000000000000000
    XAxis.Zooming.Range.High.Value = 100000000.000000000000000000
    XAxis.Zooming.Range.High.Enabled = False
    XAxis.Zooming.Range.Low.Value = 0.000000100000000000
    XAxis.Zooming.Range.Low.Enabled = True
    XAxis.AxisLabel.Font.Charset = DEFAULT_CHARSET
    XAxis.AxisLabel.Font.Color = clWhite
    XAxis.AxisLabel.Font.Height = -13
    XAxis.AxisLabel.Font.Name = 'Arial'
    XAxis.AxisLabel.Font.Style = [fsBold]
    XAxis.AxisLabel.Visible = False
    XAxis.MaxSample.Autosize = False
    XAxis.MaxSample.Value = 256
    XAxis.DataView.Lines.Pen.Color = clGreen
    XAxis.DataView.ZeroLine.Pen.Color = clWhite
    XAxis.AdditionalAxes = <>
    Legend.Align = vaRight
    Legend.Font.Charset = DEFAULT_CHARSET
    Legend.Font.Color = clWhite
    Legend.Font.Height = -11
    Legend.Font.Name = 'Arial'
    Legend.Font.Style = []
    Legend.MarkerGroups.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.MarkerGroups.Caption.Font.Color = clWhite
    Legend.MarkerGroups.Caption.Font.Height = -13
    Legend.MarkerGroups.Caption.Font.Name = 'Arial'
    Legend.MarkerGroups.Caption.Font.Style = []
    Legend.MarkerGroups.Visible = False
    Legend.Cursors.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.Cursors.Caption.Font.Color = clWhite
    Legend.Cursors.Caption.Font.Height = -13
    Legend.Cursors.Caption.Font.Name = 'Arial'
    Legend.Cursors.Caption.Font.Style = []
    Legend.CursorLinks.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.CursorLinks.Caption.Font.Color = clWhite
    Legend.CursorLinks.Caption.Font.Height = -13
    Legend.CursorLinks.Caption.Font.Name = 'Arial'
    Legend.CursorLinks.Caption.Font.Style = []
    Legend.Labels.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.Labels.Caption.Font.Color = clWhite
    Legend.Labels.Caption.Font.Height = -13
    Legend.Labels.Caption.Font.Name = 'Arial'
    Legend.Labels.Caption.Font.Style = []
    Legend.Labels.Caption.Visible = False
    Legend.Labels.Visible = False
    Legend.CustomGroups = <>
    Legend.Channels.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.Channels.Caption.Font.Color = clWhite
    Legend.Channels.Caption.Font.Height = -13
    Legend.Channels.Caption.Font.Name = 'Arial'
    Legend.Channels.Caption.Font.Style = []
    Legend.ChannelLinks.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.ChannelLinks.Caption.Font.Color = clWhite
    Legend.ChannelLinks.Caption.Font.Height = -13
    Legend.ChannelLinks.Caption.Font.Name = 'Arial'
    Legend.ChannelLinks.Caption.Font.Style = []
    Legend.Zones.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.Zones.Caption.Font.Color = clWhite
    Legend.Zones.Caption.Font.Height = -13
    Legend.Zones.Caption.Font.Name = 'Arial'
    Legend.Zones.Caption.Font.Style = []
    Legend.Ellipses.Caption.Font.Charset = DEFAULT_CHARSET
    Legend.Ellipses.Caption.Font.Color = clWhite
    Legend.Ellipses.Caption.Font.Height = -13
    Legend.Ellipses.Caption.Font.Name = 'Arial'
    Legend.Ellipses.Caption.Font.Style = []
    DataView.Border.Pen.Color = clGreen
    Highlighting.MouseHitPoint.PointLabel.Font.Charset = DEFAULT_CHARSET
    Highlighting.MouseHitPoint.PointLabel.Font.Color = clWhite
    Highlighting.MouseHitPoint.PointLabel.Font.Height = -11
    Highlighting.MouseHitPoint.PointLabel.Font.Name = 'Arial'
    Highlighting.MouseHitPoint.PointLabel.Font.Style = []
    Highlighting.Channels.HiglightLegendButton = False
    Highlighting.Channels.Color = clWhite
    MarkerGroups = <>
    Channels = <
      item
        Name = 'Channel0'
        Color = clRed
        Points.Brush.Color = clRed
        Points.Brush.Style = bsSolid
        Points.Pen.Color = clRed
        MouseCursor = crDefault
        InputPin.Form = MForm
        InputPin.SourcePin = (
          AVG.HistogramMain.OutputPins._Pin0
          'AVG.HistogramMain.OutputPins.Red')
        Synchronize = False
        Markers = <>
        AxisIndex_ = (
          0
          0)
      end
      item
        Name = 'Channel1'
        Color = clLime
        Points.Brush.Color = clRed
        Points.Brush.Style = bsSolid
        Points.Pen.Color = clRed
        MouseCursor = crDefault
        InputPin.Form = MForm
        InputPin.SourcePin = (
          AVG.HistogramMain.OutputPins._Pin1
          'AVG.HistogramMain.OutputPins.Green')
        Synchronize = False
        Markers = <>
        AxisIndex_ = (
          0
          0)
      end
      item
        Name = 'Channel2'
        Color = clBlue
        Points.Brush.Color = clRed
        Points.Brush.Style = bsSolid
        Points.Pen.Color = clRed
        MouseCursor = crDefault
        InputPin.Form = MForm
        InputPin.SourcePin = (
          AVG.HistogramMain.OutputPins._Pin2
          'AVG.HistogramMain.OutputPins.Blue')
        Synchronize = False
        Markers = <>
        AxisIndex_ = (
          0
          0)
      end>
    Zones = <>
    Ellipses = <>
    Cursors = <>
    CursorLinks = <>
    ChannelLinks = <>
    Labels = <>
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 785
    Height = 137
    Caption = #1057#1087#1080#1089#1086#1082' '#1092#1072#1081#1083#1086#1074
    TabOrder = 1
    object FileList: TListBox
      Left = 11
      Top = 16
      Width = 750
      Height = 89
      ItemHeight = 13
      TabOrder = 0
    end
    object bb_deletelist: TBitBtn
      Left = 11
      Top = 109
      Width = 25
      Height = 25
      Caption = 'd'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 1
      OnClick = btn_ListClick
    end
    object bb_deletefile: TBitBtn
      Tag = 1
      Left = 42
      Top = 109
      Width = 25
      Height = 25
      Caption = 'df'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 2
      OnClick = btn_ListClick
    end
    object bb_clearlist: TBitBtn
      Tag = 2
      Left = 73
      Top = 109
      Width = 25
      Height = 25
      Caption = 'clr'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 3
      OnClick = btn_ListClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 151
    Width = 97
    Height = 117
    Caption = #1059#1087#1088#1072#1074#1083#1077#1085#1080#1077
    TabOrder = 2
    object btn_play: TButton
      Left = 11
      Top = 24
      Width = 75
      Height = 25
      Caption = 'Play'
      TabOrder = 0
      OnClick = btn_Click
    end
    object btn_stop: TButton
      Tag = 2
      Left = 11
      Top = 77
      Width = 75
      Height = 25
      Caption = 'Stop'
      TabOrder = 2
      OnClick = btn_Click
    end
    object btn_pause: TButton
      Tag = 1
      Left = 11
      Top = 51
      Width = 75
      Height = 25
      Caption = 'Pause'
      TabOrder = 1
      OnClick = btn_Click
    end
  end
  object GroupBox3: TGroupBox
    Left = 111
    Top = 151
    Width = 682
    Height = 117
    Caption = #1057#1086#1089#1090#1086#1103#1085#1080#1077
    TabOrder = 3
    object PB_AV: TProgressBar
      Left = 11
      Top = 16
      Width = 662
      Height = 17
      Max = 512
      Step = 1
      State = pbsPaused
      TabOrder = 0
    end
    object linestatus1: TEdit
      Left = 11
      Top = 39
      Width = 662
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 1
    end
    object linestatus2: TEdit
      Left = 11
      Top = 66
      Width = 662
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 2
    end
    object linestatus3: TEdit
      Left = 11
      Top = 91
      Width = 662
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 3
    end
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 269
    Width = 785
    Height = 51
    Caption = #1055#1091#1090#1100' '#1076#1083#1103' '#1088#1077#1079#1091#1083#1100#1090#1072#1090#1086#1074
    TabOrder = 4
    object OutputFolder: TEdit
      Left = 11
      Top = 20
      Width = 734
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 0
    end
    object ButtonOutFolder: TButton
      Left = 751
      Top = 16
      Width = 25
      Height = 25
      Caption = '...'
      TabOrder = 1
      OnClick = ButtonOutFolderClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 799
    Top = 8
    Width = 60
    Height = 260
    Caption = 'Option'
    TabOrder = 7
    object SpeedButton1: TSpeedButton
      Left = 16
      Top = 16
      Width = 23
      Height = 22
      Caption = 'I'
      Transparent = False
      OnClick = OptionsClick
    end
    object SpeedButton2: TSpeedButton
      Tag = 1
      Left = 16
      Top = 44
      Width = 23
      Height = 22
      Caption = 'G'
      Transparent = False
      OnClick = OptionsClick
    end
    object SpeedButton3: TSpeedButton
      Tag = 2
      Left = 16
      Top = 72
      Width = 23
      Height = 22
      Caption = '3'
      Transparent = False
      OnClick = OptionsClick
    end
  end
  object Log: TMemo
    Left = 872
    Top = 13
    Width = 233
    Height = 255
    ReadOnly = True
    TabOrder = 8
  end
  object TimerUpdateStatus: TTimer
    OnTimer = GlobalTimer
    Left = 768
    Top = 112
  end
end
