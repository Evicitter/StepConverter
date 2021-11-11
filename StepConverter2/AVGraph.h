//Last update time : 17.07.2013 22:05:34

#ifndef AVGraphH
#define AVGraphH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LPComponent.h"
#include "MLDSPlayer.h"
#include "VLDSVideoPlayer.h"
#include "SLCommonFilter.h"
#include "VLChangeFormat.h"
#include "VLCommonFilter.h"
#include "VLHistogram.h"
#include "VLHueVector.h"
#include "VLSinkFilter.h"
#include "SLBasicGenericReal.h"
#include "SLGenericReal.h"
#include "VLBasicGenericFilter.h"
#include "VLGenericFilter.h"
#include "VLHistogramEqualize.h"
#include "VLColorSplit.h"
#include "VLColorsToReal.h"
#include "ALBasicAudioOut.h"
#include "ALDSAudioOut.h"
#include "ALRealToAudio.h"
#include "ALAudioOut.h"
#include "ALChannelMerger.h"
#include "ALChannelSplitter.h"
#include "ALCommonSplitter.h"
#include "ALMultiInput.h"
#include "ALSpectrum.h"
#include "ALAudioToReal.h"
#include "VLColorMerge.h"
#include "VLMultiInput.h"
#include "VLFrameChange.h"
#include "VLAlphaTrail.h"
#include "TLBasicTimingFilter.h"
#include "TLMainThredEvent.h"
#include "TLClockGen.h"
#include "ALBasicGenericFilter.h"
#include "ALCommonFilter.h"
#include "ALGenericFilter.h"
#include "SLBasicGenericFilter.h"
#include "SLGenericFilter.h"
#include "SLGenericRealValue.h"
#include "VLCrop.h"
#include "VLResize.h"
#include "VLFreeFrame.h"
#include "VLRealToFrame.h"
#include "VLVideoMixer.h"
#include "SLCommonGen.h"
#include "SLGenericRealGen.h"
#include "VLCombine.h"
#include "VLCommonCombine.h"
//---------------------------------------------------------------------------
class TAVG : public TDataModule
{
__published:	// IDE-managed Components
	TVLDSVideoPlayer *DSSource;
	TVLChangeFormat *CF_RGB24;
	TVLHistogram *HistogramMain;
	TVLChangeFormat *CF_RGB32;
	TVLChangeFormat *CF_UYVY;
	TVLChangeFormat *CF_YUY2;
	TVLChangeFormat *CF_YV12;
	TVLHueVector *HV_CartesianVect;
	TALChannelSplitter *ALChannelSplitter1;
	TALChannelMerger *ALChannelMerger1;
	TALSpectrum *ALSpectrumMain;
	TALAudioToReal *ALAudioToReal1;
	TALRealToAudio *ALRealToAudio1;
	TVLGenericFilter *GF_RGB24;
	TVLGenericFilter *GF_RGB32;
	TVLGenericFilter *GF_UYVY;
	TVLGenericFilter *GF_YUY2;
	TVLGenericFilter *GF_YV12;
	TVLColorSplit *CS_XYZ;
	TVLAlphaTrail *FilterTransform_1;
	TVLGenericFilter *GF_XYZ_X;
	TVLGenericFilter *GF_XYZ_Y;
	TVLGenericFilter *GF_XYZ_Z;
	TSLGenericReal *GR_HVC_X;
	TSLGenericReal *GR_HVC_Y;
	TSLGenericReal *GR_HVC_L;
	TVLHueVector *HV_PolarVect;
	TSLGenericReal *GR_HVP_M;
	TSLGenericReal *GR_HVP_P;
	TSLGenericReal *GR_HVP_L;
	TVLResize *FilterTransform_2;
	TVLChangeFormat *CF_FrameSuit;
	TVLGenericFilter *GF_FrameSuit;
	TALGenericFilter *ALGF_Center;
	TTLClockGen *MainClockGen;
	TVLCombine *VLCombine1;
	void __fastcall GenericFilter_Start(TObject *Sender, int &AWidth, int &AHeight,
          float SampleRate);
	void __fastcall GenericFilter_Stop(TObject *Sender);
	void __fastcall GenericFilter_Process(TObject *Sender, TVLCVideoBuffer InBuffer,
          TVLCVideoBuffer &OutBuffer, bool &SendOutputData);
	void __fastcall GenericRealHV_Start(TObject *Sender, double SampleRate);
	void __fastcall GenericRealHV_Stop(TObject *Sender);
	void __fastcall GenericRealHV_Process(TObject *Sender, TSLCRealBuffer InBuffer,
          TSLCRealBuffer &OutBuffer, bool &SendOutputData);
	void __fastcall DSSourceProgress(TObject *Sender, __int64 StartSample, __int64 EndSample,
          __int64 CurrentSample);
	void __fastcall GenericFilterSuit_Start(TObject *Sender, int &AWidth, int &AHeight,
          float SampleRate);
	void __fastcall GenericFilterSuit_Stop(TObject *Sender);
	void __fastcall GenericFilterSuit_Process(TObject *Sender, TVLCVideoBuffer InBuffer,
          TVLCVideoBuffer &OutBuffer, bool &SendOutputData);
	void __fastcall ALGF_Start(TObject *Sender);
	void __fastcall ALGF_Stop(TObject *Sender);
	void __fastcall ALGF_Process(TObject *Sender, TALCAudioBuffer InBuffer, TALCAudioBuffer &OutBuffer,
		  bool &SendOutputData);
	void __fastcall DSSourceStop(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TAVG(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAVG *AVG;
//---------------------------------------------------------------------------

extern unsigned int LsFrame;
extern __int64 EFrame;
extern __int64 CFrame;
extern __int64 LFrame;
extern int CurrentProgressPos;
extern int LastProgressPos;
extern unsigned int Counts[64];
extern unsigned int CountsSuit[64];
extern unsigned int CountsAL;
extern UnicodeString FolderOut;
extern UnicodeString FileOut;

extern int ABufferSize;
extern void __fastcall AudioAlarm(void* buff, int size);
#endif