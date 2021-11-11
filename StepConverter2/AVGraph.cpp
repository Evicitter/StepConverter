//Last update time : 17.07.2013 22:07:44

#include <vcl.h>
#pragma hdrstop

#include "AVGraph.h"
#include "MainForm.h"

#include <stdio.h>

#include "hashlib.h"
#pragma comment (lib,"hashlib-bcb.lib")

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LPComponent"
#pragma link "MLDSPlayer"
#pragma link "VLDSVideoPlayer"
#pragma link "SLCommonFilter"
#pragma link "VLChangeFormat"
#pragma link "VLCommonFilter"
#pragma link "VLHistogram"
#pragma link "VLHueVector"
#pragma link "VLSinkFilter"
#pragma link "SLBasicGenericReal"
#pragma link "SLGenericReal"
#pragma link "VLBasicGenericFilter"
#pragma link "VLGenericFilter"
#pragma link "VLHistogramEqualize"
#pragma link "VLColorSplit"
#pragma link "VLColorsToReal"
#pragma link "ALBasicAudioOut"
#pragma link "ALDSAudioOut"
#pragma link "ALRealToAudio"
#pragma link "ALAudioOut"
#pragma link "ALChannelSplitter"
#pragma link "ALCommonSplitter"
#pragma link "ALMultiInput"
#pragma link "ALSpectrum"
#pragma link "ALAudioToReal"
#pragma link "VLColorMerge"
#pragma link "VLMultiInput"
#pragma link "VLFrameChange"
#pragma link "VLAlphaTrail"
#pragma link "TLBasicTimingFilter"
#pragma link "TLMainThredEvent"
#pragma link "TLClockGen"
#pragma link "ALBasicGenericFilter"
#pragma link "ALCommonFilter"
#pragma link "ALGenericFilter"
#pragma link "SLBasicGenericFilter"
#pragma link "SLGenericFilter"
#pragma link "SLGenericRealValue"
#pragma link "VLCrop"
#pragma link "VLResize"
#pragma link "VLFreeFrame"
#pragma link "VLRealToFrame"
#pragma link "VLVideoMixer"
#pragma link "SLCommonGen"
#pragma link "SLGenericRealGen"
#pragma link "VLCombine"
#pragma link "VLCommonCombine"
#pragma resource "*.dfm"
TAVG *AVG;

int ABufferSize=0;
unsigned int LsFrame=0;
unsigned int Counts[64]={0};
unsigned int CountsSuit[64]={0};    //reserved
unsigned int CountsAL=0;
UnicodeString FolderOut;
UnicodeString FileOut;

__int64 EFrame=0i64;
__int64 CFrame=0i64;
__int64 LFrame=0i64;
int CurrentProgressPos=0;
int LastProgressPos=0;
int CounterHashFrames=0;
//int CounterHashComponents=0;
FILE* FileHashFrames=NULL;
FILE* FileHashSamples=NULL;
FILE* FileMapFrames=NULL;

float* outDelta2000b=NULL;

class BuffAccel
{
public:
	void* buffer;
	int channels;
	int bytes;
	int samples;
	int current;

	void (__fastcall *alarm)(void* buffer, int size);

	BuffAccel() : buffer(NULL), channels(1), bytes(1), samples(0), current(0), alarm(AudioAlarm) {}
	~BuffAccel() { if(buffer!=NULL) buffer=realloc(buffer,0); }
	void __fastcall set(int nbytes, int nsamples, int nchannels)
	{
	  if((nsamples!=samples) && (nbytes!=bytes) && (nchannels!=channels))
	  {
	   buffer = realloc(buffer, nsamples * nbytes * nchannels );
	   current=0;
	   channels=nchannels;
	   bytes=nbytes;
	   samples=nsamples;
	  }
	}

	void __fastcall add(void* newbuffer, int len)
	{
	  unsigned int lenbyte = bytes*channels;
	  unsigned int fill;
	  unsigned long left;

	  if( len == 0 ) return;  //-check
	  len /= bytes*channels;

	  left = current % samples;
	  fill = samples - left;

	  current += len;

	if( left && (len >= fill) )
	{
		memcpy((unsigned char*)buffer + left, newbuffer, fill*lenbyte);
		alarm(buffer,samples*lenbyte);
		newbuffer = (char*)newbuffer + fill*lenbyte;
		len -= fill;
		left = 0;
	}

	while( len >= samples )
	{
		alarm(newbuffer,samples*lenbyte);
		newbuffer = (char*)newbuffer + samples*lenbyte;
		len -= samples;
	}

	if( len > 0 ) memcpy((unsigned char*)buffer + left*bytes, newbuffer, len*lenbyte);
	}
};

BuffAccel audioAB;

//---------------------------------------------------------------------------
__fastcall TAVG::TAVG(TComponent* Owner)
	: TDataModule(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TAVG::DSSourceProgress(TObject *Sender, __int64 StartSample, __int64 EndSample,
		  __int64 CurrentSample)
{
// CurrentProgressPos = CurrentSample * 512 / EndSample; //(EndSample - StartSample);
 CFrame = CurrentSample;
 EFrame = EndSample;
}

void __fastcall TAVG::DSSourceStop(TObject *Sender)
{
 MainClockGen->Enabled = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TAVG::GenericFilter_Start(TObject *Sender, int &AWidth, int &AHeight, float SampleRate)
{
 TVLGenericFilter* gf = dynamic_cast<TVLGenericFilter*>(Sender);
 if(!gf) return;
 if(!gf->Enabled) return;

 if( CounterHashFrames == 0 )
 {
   if(FileHashFrames != NULL)
   {
	closeFile(FileHashFrames);
	FileHashFrames = NULL;
   }
   mframecache.vRH.clear();
   mframecache.vRH64.clear();
   mframecache.vADM.clear();
   mframecache.vAHI.clear();

   UnicodeString us = FolderOut + ExtractFileName(FileOut) + L".fbin";
   FileHashFrames = (FILE*)createFile(us.w_str());
 }

 CounterHashFrames = InterlockedIncrement((long*)&CounterHashFrames); //++CounterHashFrames;
 Counts[gf->Tag]=0;
}
//---------------------------------------------------------------------------

//void __fastcall TAVG::GenericFilter_Stop(TObject *Sender) {}

void __fastcall TAVG::GenericFilter_Stop(TObject *Sender)
{
 TVLGenericFilter* gf = dynamic_cast<TVLGenericFilter*>(Sender);
 if(!gf) return;
 if(!gf->Enabled) return;

 CounterHashFrames = InterlockedDecrement((long*)&CounterHashFrames); //--CounterHashFrames;

 if(CounterHashFrames==0)
 {
   if(FileHashFrames != NULL)
   {
    GroupHashWrite(NULL, 0,(void*)FileHashFrames, &mframecache, true);
	closeFile((FILE*)FileHashFrames);
	FileHashFrames=NULL;
   }
 }
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericFilter_Process(TObject *Sender, TVLCVideoBuffer InBuffer,
		  TVLCVideoBuffer &OutBuffer, bool &SendOutputData)
{
 SendOutputData = false;

 TVLGenericFilter* gf = dynamic_cast<TVLGenericFilter*>(Sender);
 if(!gf) return;

 //security --------------------------------------------------------------------
 //if(tempFile[gf->Tag] == NULL) return;
 if(FileHashFrames==NULL) return;
 //-----------------------------------------------------------------------------

 GroupHashWrite((void*)(InBuffer.Read()), InBuffer.GetByteSize() ,(void*)FileHashFrames, &mframecache);
 ++Counts[gf->Tag];
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericFilterSuit_Start(TObject *Sender, int &AWidth, int &AHeight,
		  float SampleRate)
{
 TVLGenericFilter* gf = dynamic_cast<TVLGenericFilter*>(Sender);
 if(!gf) return;
 if(!gf->Enabled) return;

 CountsSuit[0] = 0;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericFilterSuit_Stop(TObject *Sender)
{
 TVLGenericFilter* gf = dynamic_cast<TVLGenericFilter*>(Sender);
 if(!gf) return;
 if(!gf->Enabled) return;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericFilterSuit_Process(TObject *Sender, TVLCVideoBuffer InBuffer,
          TVLCVideoBuffer &OutBuffer, bool &SendOutputData)
{
  SendOutputData = false;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericRealHV_Start(TObject *Sender, double SampleRate)
{
   TSLGenericReal* gr = dynamic_cast<TSLGenericReal*>(Sender);
   if(!gr) return;
   if(!gr->Enabled) return;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericRealHV_Stop(TObject *Sender)
{
   TSLGenericReal* gr = dynamic_cast<TSLGenericReal*>(Sender);
   if(!gr) return;
   if(!gr->Enabled) return;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::GenericRealHV_Process(TObject *Sender, TSLCRealBuffer InBuffer,
		  TSLCRealBuffer &OutBuffer, bool &SendOutputData)
{
   SendOutputData=false;
   TSLGenericReal* gr = dynamic_cast<TSLGenericReal*>(Sender);
   if(!gr) return;
}
//---------------------------------------------------------------------------

void __fastcall TAVG::ALGF_Start(TObject *Sender)
{
 TALGenericFilter* algf = dynamic_cast<TALGenericFilter*>(Sender);
 if(algf == NULL) return;
 if(!algf->Enabled) return;

 if(FileHashSamples != NULL)
 {
  closeFile(FileHashSamples);
  FileHashSamples=NULL;
 }

 CountsAL=0;

  msamplecache.vRH.clear();
  msamplecache.vRH64.clear();
  msamplecache.vADM.clear();
  msamplecache.vAHI.clear();

 //open dest file results ------------------------------------------------------
 UnicodeString us = FolderOut + ExtractFileName(FileOut) + L".sbin";
 FileHashSamples = (FILE*)createFile(us.w_str());
 //-----------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

void __fastcall TAVG::ALGF_Stop(TObject *Sender)
{
  TALGenericFilter* algf = dynamic_cast<TALGenericFilter*>(Sender);
 if(algf == NULL) return;
 if(!algf->Enabled) return;
 if(FileHashSamples!=NULL)
 {
  GroupHashWrite(NULL, 0, FileHashSamples, &msamplecache, true);
  closeFile(FileHashSamples);
  FileHashSamples=NULL;
 }
}

void __fastcall AudioAlarm(void* buff, int size)
{
 GroupHashWrite(buff, size, FileHashSamples, &msamplecache);
}

void __fastcall TAVG::ALGF_Process(TObject *Sender, TALCAudioBuffer InBuffer, TALCAudioBuffer &OutBuffer,
		  bool &SendOutputData)
{
 SendOutputData=false;

 //security ----------------------------------
 if(FileHashSamples == NULL) return;
 //-------------------------------------------

 audioAB.set( InBuffer.GetBitsPerSample() / 8, InBuffer.GetSamplesPerSecond(), InBuffer.GetChannels());

 const unsigned char* p = InBuffer.Read();
 const unsigned int size = InBuffer.GetByteSize();

 audioAB.add((void*)p, size);

 ++CountsAL;
}
//---------------------------------------------------------------------------