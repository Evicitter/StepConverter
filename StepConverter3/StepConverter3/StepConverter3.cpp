//Last update time : 06.11.2013 13:49:24
#include "stdafx.h"
#include "ffmpegDecode.h"
#include "IniReader.h"
#include "IniWriter.h"

#include "HashClasses.h"
#include "HashCollection.h"

#include "binFile.h"

#include <vector>
using namespace std;

#include <windows.h>

#define INPUT_FILE_NAME    argv[1] //L"C:\\test.avi"
#define OUTPUT_FILE_PREFIX L"C:\\image%dt%i.bmp"
#define FRAME_COUNT        200

wchar_t outputfolder[1024]={L'\0'};
wchar_t workoutfile[1024]={L'\0'};
wchar_t workoutfileGRB[1024]={L'\0'};

//GLOBAL VARIABLES -------------------------------------
binFile gBinFileWrite;
binFile gBinFileRead;
//------------------------------------------------------

//int height;
//int width;

bool __fastcall getValidVideoFileName(wchar_t* fname)
{
	const wchar_t* fileext = wcsrchr(fname, L'.');
	if(fileext!=NULL)
	{
		bool bset =    (_wcsicmp(fileext, L".avi") == 0);
		bset = bset || (_wcsicmp(fileext, L".mp4") == 0);
		bset = bset || (_wcsicmp(fileext, L".wmv") == 0);
		bset = bset || (_wcsicmp(fileext, L".mkv") == 0);
		bset = bset || (_wcsicmp(fileext, L".mpeg")== 0);
		bset = bset || (_wcsicmp(fileext, L".mpg") == 0);
		bset = bset || (_wcsicmp(fileext, L".mov") == 0);
		bset = bset || (_wcsicmp(fileext, L".flv") == 0);
		bset = bset || (_wcsicmp(fileext, L".ts")  == 0);
		bset = bset || (_wcsicmp(fileext, L".m2ts")== 0);
		bset = bset || (_wcsicmp(fileext, L".ogv") == 0);
		bset = bset || (_wcsicmp(fileext, L".divx")== 0);
		bset = bset || (_wcsicmp(fileext, L".m4v") == 0);
		bset = bset || (_wcsicmp(fileext, L".3gp") == 0);
		//-------------------------------------------------
		bset = bset || (_wcsicmp(fileext, L".ac3") == 0);
		bset = bset || (_wcsicmp(fileext, L".wav") == 0);
		return bset;
    }
	return false;
}

bool __fastcall getValidOtherFile(wchar_t* fname)
{
	const wchar_t* fileext = wcsrchr(fname, L'.');
	if(fileext!=NULL)
	{
		bool bset =    (_wcsicmp(fileext, L".grb") == 0);
		return bset;
	}
	return false;
}

void getvideoframe(const unsigned char* bytes, const int size, const int typeframe)
{
    //wprintf_s(L"video frame (%i) (%i)\n", size, typeframe);
	//unsigned char chash[16];
	PushHash(&gBinFileWrite, bytes, size);
	//wprintf_s(L"fideoframe(%i) = %0.8I64x%0.8I64x\n", typeframe, *(unsigned long long*)(chash.mm+8), *(unsigned long long*)chash.mm);
}

void getaudioframe(const unsigned char* bytes, const int size, const int typeframe)
{
	//wprintf_s(L"audio frame (%i) (%i)\n", size, typeframe);
	PushHash(&gBinFileWrite, bytes, size);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<2) return 0;
	if(GetFileAttributes(INPUT_FILE_NAME) == -1) return 0;

	_wsetlocale(LC_ALL, L"RUS");

	CIniReader inir(L"config.ini");
	CIniWriter iniw(L"config.ini");

	//read config	------------------------------------------------------------
	inir.ReadStringOut(L"main",L"outdir",L"C:\\",outputfolder,1024);
	//--------------------------------------------------------------------------

	//set priority
	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
	//-------------

	//open binary file write
	 wcscpy_s(workoutfile,1024,outputfolder);
	 wcscat_s(workoutfile,1024,L"\\");
	 const wchar_t* filename=wcsrchr(INPUT_FILE_NAME, L'\\');
	 if(filename!=NULL)
	 { 
		 wcscat_s(workoutfile,1024,filename+1);
		 wcscpy_s(workoutfileGRB, 1024, workoutfile);
		 wcscat_s(workoutfile,1024,L".gbin");
		 wcscat_s(workoutfileGRB,1024,L"grb.gbin");
	 }
	 else
	 {
	  wcscat_s(workoutfile,1024,L"GlobalHash.gbin");
	  wcscat_s(workoutfileGRB,1024,L"GlobalHashGRB.gbin");
	 }

	 
	//--------------------------------------------------------------------------
   if( getValidVideoFileName(INPUT_FILE_NAME) )
   {
	FFmpegDecoder decoder;

	if (decoder.OpenFile(std::wstring(INPUT_FILE_NAME)))
	{
	 gBinFileWrite.saveFile(workoutfile);

	 if(decoder.VideoDecoderInstance!=NULL)
	 {
	  decoder.VideoDecoderInstance->notifyvideoframe = getvideoframe;
	  int mwidth = decoder.VideoDecoderInstance->GetWidth();
      int mheight = decoder.VideoDecoderInstance->GetHeight();
	 }
	 if(decoder.AudioDecoderInstance!=NULL)
	 {
		 decoder.AudioDecoderInstance->notifyaudioframe = getaudioframe;
	 }
	 
	 PushHash(&gBinFileWrite, NULL, 0, true);

	 //for (int i = 0; i < FRAME_COUNT; i++)	decoder.GetNextFrame();
	 //while(decoder.GetNextFrame()>=0);
	 decoder.StartWork();

     decoder.CloseFile();
 
	 gBinFileWrite.closeFile();

	 //get grb this file
	  int blocksize = inir.ReadInteger(L"main",L"readBlock_AV",256);
	  blocksize = (blocksize<128) ? 128 : blocksize;
	  blocksize = (blocksize>65536) ? 65536 : blocksize;
	  getGRB(INPUT_FILE_NAME, workoutfileGRB, &gBinFileWrite, blocksize);
	  iniw.WriteInteger(L"main",L"readBlock_AV",blocksize);
	  //-------------------------------------------------------------------

	 _wremove(INPUT_FILE_NAME);
	}
	else
	{
		wprintf_s (L"Cannot open file %s\n", INPUT_FILE_NAME);
	}
  }
  else if( getValidOtherFile(INPUT_FILE_NAME) )
  {
	  int blocksize = inir.ReadInteger(L"main",L"readBlock_GRB",256);
	  blocksize = (blocksize<128) ? 128 : blocksize;
	  blocksize = (blocksize>65536) ? 65536 : blocksize;
	  getGRB(INPUT_FILE_NAME, workoutfileGRB, &gBinFileWrite, blocksize);
	  iniw.WriteInteger(L"main",L"readBlock_GRB",blocksize);
	  _wremove(INPUT_FILE_NAME);
  }

  iniw.WriteString(L"main",L"outdir",outputfolder);

  return 0;
}