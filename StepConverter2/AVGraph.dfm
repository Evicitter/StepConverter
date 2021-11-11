object AVG: TAVG
  OldCreateOrder = False
  Height = 511
  Width = 804
  object DSSource: TVLDSVideoPlayer
    ClockPin.Form = AVG
    ClockPin.SourcePin = AVG.MainClockGen.OutputPin
    Graph.AdditionalFilters = <>
    Enabled = False
    ClockSource = csExternal
    RestartOnNewFile = False
    PlaybackRate.Rate = 1.000000000000000000
    OnProgress = DSSourceProgress
    OnStop = DSSourceStop
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.FilterTransform_1.InputPin)
    AudioOutputPin.Form = AVG
    AudioOutputPin.SinkPins = (
      AVG.ALGF_Center.InputPin)
    Left = 72
    Top = 8
  end
  object CF_RGB24: TVLChangeFormat
    Threading.Enabled = True
    Threading.Queue.Size = 150
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_RGB24.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    Left = 32
    Top = 104
  end
  object HistogramMain: TVLHistogram
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_2.OutputPin
    OutputPins.Form = AVG
    OutputPins.Pins = (
      Sinks
      (
        (
          MForm.SLScope1.InputPins._Pin0
          'MForm.SLScope1.InputPins.Channel0'))
      Sinks
      (
        (
          MForm.SLScope1.InputPins._Pin1
          'MForm.SLScope1.InputPins.Channel1'))
      Sinks
      (
        (
          MForm.SLScope1.InputPins._Pin2
          'MForm.SLScope1.InputPins.Channel2'))
      nil)
    Left = 736
    Top = 400
  end
  object CF_RGB32: TVLChangeFormat
    Tag = 1
    Threading.Enabled = True
    Threading.Queue.Size = 150
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_RGB32.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    Format = vfRGBA32
    Left = 88
    Top = 104
  end
  object CF_UYVY: TVLChangeFormat
    Tag = 6
    Threading.Enabled = True
    Threading.Queue.Size = 150
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_UYVY.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    Format = vfUYVY
    Left = 216
    Top = 104
  end
  object CF_YUY2: TVLChangeFormat
    Tag = 7
    Threading.Enabled = True
    Threading.Queue.Size = 150
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_YUY2.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    Format = vfYUY2
    Left = 272
    Top = 104
  end
  object CF_YV12: TVLChangeFormat
    Tag = 8
    Threading.Enabled = True
    Threading.Queue.Size = 150
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_YV12.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    Format = vfYV12
    Left = 328
    Top = 104
  end
  object HV_CartesianVect: TVLHueVector
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    OutputPins.Form = AVG
    OutputPins.Pins = (
      Sinks
      (
        AVG.GR_HVC_X.InputPin)
      Sinks
      (
        AVG.GR_HVC_Y.InputPin)
      Sinks
      (
        AVG.GR_HVC_L.InputPin))
    Left = 432
    Top = 400
  end
  object ALChannelSplitter1: TALChannelSplitter
    OutputPins.Form = AVG
    Left = 712
    Top = 208
  end
  object ALChannelMerger1: TALChannelMerger
    InputPins.Form = AVG
    Left = 712
    Top = 264
  end
  object ALSpectrumMain: TALSpectrum
    CountChannels = 1
    OutputPins.Form = AVG
    Normalization = fnNone
    Synchronize = False
    FrequencyOutputPins.Form = AVG
    Left = 568
    Top = 24
  end
  object ALAudioToReal1: TALAudioToReal
    OutputPins.Form = AVG
    Left = 624
    Top = 144
  end
  object ALRealToAudio1: TALRealToAudio
    InputPins.Form = AVG
    Left = 712
    Top = 144
  end
  object GF_RGB24: TVLGenericFilter
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_RGB24.OutputPin
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 32
    Top = 160
  end
  object GF_RGB32: TVLGenericFilter
    Tag = 1
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_RGB32.OutputPin
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 88
    Top = 160
  end
  object GF_UYVY: TVLGenericFilter
    Tag = 6
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_UYVY.OutputPin
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 216
    Top = 160
  end
  object GF_YUY2: TVLGenericFilter
    Tag = 7
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_YUY2.OutputPin
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 272
    Top = 160
  end
  object GF_YV12: TVLGenericFilter
    Tag = 8
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_YV12.OutputPin
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 328
    Top = 160
  end
  object CS_XYZ: TVLColorSplit
    Threading.Enabled = True
    Threading.Queue.Size = 150
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    OutputPins.Form = AVG
    OutputPins.Pins = (
      Sinks
      (
        AVG.GF_XYZ_X.InputPin)
      Sinks
      (
        AVG.GF_XYZ_Y.InputPin)
      Sinks
      (
        AVG.GF_XYZ_Z.InputPin)
      nil)
    ColorMode = cmXYZ
    Left = 88
    Top = 232
  end
  object FilterTransform_1: TVLAlphaTrail
    Enabled = False
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.CF_RGB24.InputPin
      AVG.CF_RGB32.InputPin
      AVG.CF_UYVY.InputPin
      AVG.HV_CartesianVect.InputPin
      AVG.CS_XYZ.InputPin
      AVG.CF_YUY2.InputPin
      AVG.CF_YV12.InputPin
      AVG.HV_PolarVect.InputPin
      AVG.FilterTransform_2.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.DSSource.OutputPin
    Alpha = 0.779999971389770500
    Left = 144
    Top = 8
  end
  object GF_XYZ_X: TVLGenericFilter
    Tag = 28
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.CS_XYZ.OutputPins._Pin0
      'AVG.CS_XYZ.OutputPins.X')
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 88
    Top = 288
  end
  object GF_XYZ_Y: TVLGenericFilter
    Tag = 29
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.CS_XYZ.OutputPins._Pin1
      'AVG.CS_XYZ.OutputPins.Y')
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 88
    Top = 344
  end
  object GF_XYZ_Z: TVLGenericFilter
    Tag = 30
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.CS_XYZ.OutputPins._Pin2
      'AVG.CS_XYZ.OutputPins.Z')
    OnStop = GenericFilter_Stop
    OnStart = GenericFilter_Start
    OnProcessData = GenericFilter_Process
    Left = 88
    Top = 408
  end
  object GR_HVC_X: TSLGenericReal
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_CartesianVect.OutputPins._Pin0
      'AVG.HV_CartesianVect.OutputPins.X')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 528
    Top = 400
  end
  object GR_HVC_Y: TSLGenericReal
    Tag = 1
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_CartesianVect.OutputPins._Pin1
      'AVG.HV_CartesianVect.OutputPins.Y')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 592
    Top = 400
  end
  object GR_HVC_L: TSLGenericReal
    Tag = 2
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_CartesianVect.OutputPins._Pin2
      'AVG.HV_CartesianVect.OutputPins.Lightness')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 656
    Top = 400
  end
  object HV_PolarVect: TVLHueVector
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    OutputPins.Form = AVG
    OutputPins.Pins = (
      Sinks
      (
        AVG.GR_HVP_M.InputPin)
      Sinks
      (
        AVG.GR_HVP_P.InputPin)
      Sinks
      (
        AVG.GR_HVP_L.InputPin))
    Mode = vmPolar
    Left = 432
    Top = 448
  end
  object GR_HVP_M: TSLGenericReal
    Tag = 3
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_PolarVect.OutputPins._Pin0
      'AVG.HV_PolarVect.OutputPins.Magnitude')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 528
    Top = 448
  end
  object GR_HVP_P: TSLGenericReal
    Tag = 4
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_PolarVect.OutputPins._Pin1
      'AVG.HV_PolarVect.OutputPins.Phase')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 592
    Top = 448
  end
  object GR_HVP_L: TSLGenericReal
    Tag = 5
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = (
      AVG.HV_PolarVect.OutputPins._Pin2
      'AVG.HV_PolarVect.OutputPins.Lightness')
    OnStop = GenericRealHV_Stop
    OnStart = GenericRealHV_Start
    OnProcessData = GenericRealHV_Process
    Left = 656
    Top = 448
  end
  object FilterTransform_2: TVLResize
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.HistogramMain.InputPin
      MForm.VLDSImageDisplay1.InputPin
      AVG.CF_FrameSuit.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_1.OutputPin
    InterpolationType = itNearestNeighbor
    BackgroundColor = clYellow
    OutputRegion.Width = 256
    OutputRegion.Height = 256
    Width = 256
    Height = 256
    Left = 232
    Top = 8
  end
  object CF_FrameSuit: TVLChangeFormat
    Enabled = False
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.GF_FrameSuit.InputPin)
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.FilterTransform_2.OutputPin
    Format = vfRGBA32
    Left = 232
    Top = 232
  end
  object GF_FrameSuit: TVLGenericFilter
    Enabled = False
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.CF_FrameSuit.OutputPin
    OnStop = GenericFilterSuit_Stop
    OnStart = GenericFilterSuit_Start
    OnProcessData = GenericFilterSuit_Process
    Left = 232
    Top = 288
  end
  object ALGF_Center: TALGenericFilter
    InputPin.Form = AVG
    InputPin.SourcePin = AVG.DSSource.AudioOutputPin
    OnStart = ALGF_Start
    OnStop = ALGF_Stop
    OnProcessData = ALGF_Process
    Left = 488
    Top = 24
  end
  object MainClockGen: TTLClockGen
    Enabled = False
    OutputPin.Form = AVG
    OutputPin.SinkPins = (
      AVG.DSSource.ClockPin)
    Mode = tcmFullSpeed
    Rate = 30.000000000000000000
    Left = 72
    Top = 56
  end
  object VLCombine1: TVLCombine
    BackgroundColor = 0
    Coefficient1 = 1.000000000000000000
    Coefficient2 = 1.000000000000000000
    InputPins.Form = AVG
    Left = 712
    Top = 328
  end
end
