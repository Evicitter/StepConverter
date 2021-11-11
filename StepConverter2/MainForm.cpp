//Last update time : 11.10.2013 16:59:36

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "AVGraph.h"

#include <FileCtrl.hpp>

#include <stdio.h>
#include <process.h>

#include "hashlib.h"
#include "NetModule.h"

//ini cinfiguration
#include "IniReader.h"
#include "IniWriter.h"
//----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LPComponent"
#pragma link "SLCommonFilter"
#pragma link "SLCommonGen"
#pragma link "VLColorTwist"
#pragma link "VLCommonFilter"
#pragma link "VLCommonGen"
#pragma link "VLJaehneGen"
#pragma link "LPControl"
#pragma link "SLControlCollection"
#pragma link "VLCommonDisplay"
#pragma link "VLImageDisplay"
#pragma link "VLDSImageDisplay"
#pragma link "LPControlDrawLayers"
#pragma link "SLScope"
#pragma resource "*.dfm"
TMForm *MForm;

MMvectors mgrbcache; //not use
MMvectors mframecache;
MMvectors msamplecache;

// unsigned char xCount[37]={0};
unsigned int cclocal = 7;

void __fastcall GroupHashWrite(void* buffer,const int size, void* hashfile, MMvectors* mcache, bool bFinal)
{
 if((mcache==NULL) || (hashfile==NULL)) return;
 if(bFinal)
 {
  if(!mcache->vRH.empty())
  {
   writeChunkElement(hashfile, MAKE8CC('R','H','A','S','H','\0','\0','\0'), (unsigned int)(mcache->vRH.size()), 16ui16, (char*)&(mcache->vRH[0]));
   mcache->vRH.clear();
  }

  if(!mcache->vRH64.empty())
  {
   writeChunkElement(hashfile, MAKE8CC('R','H','A','S','H','6','4','\0'), (unsigned int)(mcache->vRH64.size()), 8ui16, (char*)&(mcache->vRH64[0]));
   mcache->vRH64.clear();
  }

  if(!mcache->vADM.empty())
  {
   writeChunkElement(hashfile, MAKE8CC('A','E','S','_','D','M','\0','\0'), (unsigned int)(mcache->vADM.size()), 32ui16, (char*)&(mcache->vADM[0]));
   mcache->vADM.clear();
  }

  if(!mcache->vAHI.empty())
  {
   writeChunkElement(hashfile, MAKE8CC('A','E','S','_','H','I','\0','\0'), (unsigned int)(mcache->vAHI.size()), 32ui16, (char*)&(mcache->vAHI[0]));
   mcache->vAHI.clear();
  }
  return;
 }

 //md5_context md5c;
 rhash_context rhc;
 rhash64_context rh64c;
 aes_context aesc;
 aes_context_complex aescc;

 //initialization
 //mmxMD5_s(&md5c);
 mmxRHash_s( &rhc );
 mmxRHash64_s( &rh64c );
 mmxAES_DM_s( &aesc );
 mmxAES_complex_s( &aescc );

 mmxRHash_u		( &rhc,  buffer, size );
 mmxRHash64_u	( &rh64c, buffer, size );
 mmxAES_DM_u	( &aesc, buffer, size );
 mmxAES_complex_u( &aescc, buffer, size );

 //finalize  -----------------------------
 //unsigned char d[16];
 mm16 d;
 //mmxMD5_f(&md5c, d);
 //writeChunkElement(hashfile, MAKE8CC('M','D','5','\0','\0','\0','\0','\0'), 16, d);

 mmxRHash_f(&rhc, d.val);
 mcache->vRH.push_back(d);

 if(mcache->vRH.size() >= 262144)  //4MB div 16
 {
  writeChunkElement(hashfile, MAKE8CC('R','H','A','S','H','\0','\0','\0'), (unsigned int)(mcache->vRH.size()), 16ui16, (char*)&(mcache->vRH[0]));
  mcache->vRH.clear();
 }

 mmxRHash64_f(&rh64c, d.val);
 mcache->vRH64.push_back( (__int64)d );

 if(mcache->vRH64.size() >= 524288) //4MB div 8
 {
  writeChunkElement(hashfile, MAKE8CC('R','H','A','S','H','6','4','\0'), (unsigned int)(mcache->vRH64.size()), 8ui16, (char*)&(mcache->vRH64[0]));
  mcache->vRH64.clear();
 }

 mm32 dd;
 mmxAES_DM_f(&aesc);
 mmxAES_complex_f(&aescc);

 memcpy(dd.val, aesc.state, 32);		mcache->vADM.push_back( dd );
 memcpy(dd.val, aescc.state_HI, 32);	mcache->vAHI.push_back( dd );

 if(mcache->vADM.size() >= 131072) //4MB div 32
 {
  writeChunkElement(hashfile, MAKE8CC('A','E','S','_','D','M','\0','\0'), (unsigned int)(mcache->vADM.size()), 32ui16, (char*)&(mcache->vADM[0]));
  mcache->vADM.clear();
 }

 //writeChunkElement(hashfile, MAKE8CC('A','E','S','_','2','\0','\0','\0'), 16, aescc.state_MMO);
 //writeChunkElement(hashfile, MAKE8CC('A','E','S','_','3','\0','\0','\0'), 16, aescc.state_MP);

 if(mcache->vAHI.size() >= 131072) //4MB div 32
 {
  writeChunkElement(hashfile, MAKE8CC('A','E','S','_','H','I','\0','\0'), (unsigned int)(mcache->vAHI.size()), 32ui16, (char*)&(mcache->vAHI[0]));
  mcache->vAHI.clear();
 }
}

bool __fastcall getValidFileName(wchar_t* fname)
{
	wchar_t* fileext = wcsrchr(fname, L'.');
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
		bset = bset || (_wcsicmp(fileext, L".grb") == 0);
		return bset;
    }
	return false;
}

// ---------------------------------------------------------------------------
__fastcall TMForm::TMForm(TComponent* Owner) : TForm(Owner) {}
// ---------------------------------------------------------------------------

void __fastcall TMForm::WmDropFiles(TWMDropFiles& Message) {
	if (Message.Msg == WM_DROPFILES) {
		HDROP drop_handle = (HDROP)Message.Drop;
		wchar_t fName[1024];
		int filenum = DragQueryFile(drop_handle, -1, NULL, NULL);
		for (int i = 0; i < filenum; i++)
		{
			DragQueryFile(drop_handle, i, fName, 1024);
			if( getValidFileName(fName) ) FileList->Items->Add(fName);
		}
		DragFinish(drop_handle);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMForm::btn_Click(TObject *Sender) {
	TButton* tb = dynamic_cast<TButton*>(Sender);
	if (!tb)
		return;

	switch(tb->Tag) {
	case 0:
		if (FileList->Count <= 0)			return;
		if (FileList->ItemIndex < 0)		return;

		FileOut = FileList->Items[0][FileList->ItemIndex];
		//if( ExtractFileExt(FileOut) )

		AVG->DSSource->FileName = FileOut;
		AVG->DSSource->Open();
		AVG->DSSource->Start();
		AVG->MainClockGen->Enabled = true;

		break;
	case 1:
		if(AVG->DSSource->Paused)
		{
			AVG->DSSource->Resume();
			PB_AV->State = TProgressBarState::pbsPaused; //only style for visual
		} else
		{
			AVG->DSSource->Pause();
			PB_AV->State = TProgressBarState::pbsError;  //only style for visual
        }
		break;
	case 2:

		AVG->DSSource->Stop();
		AVG->DSSource->Close();
		AVG->DSSource->FileName = L"";

		AVG->MainClockGen->Enabled = false;
		AVG->DSSource->Enabled = false;

		break;
	}
}

// ---------------------------------------------------------------------------

void __fastcall TMForm::FormCreate(TObject *Sender)
{
 DragAcceptFiles(Handle, true);
 CIniReader ir(L"Options.ini");
 //{ load output folder settings
	 UnicodeString way = ir.ReadString(L"mform", L"outputfolder", L"");
	 OutputFolder->Text = way;
	 FolderOut = way;
 //}
 //UnicodeString us = GetCommandLineW();
/* int pcount = ParamCount();
 for(int i=1;i<pcount;++i)
 {
  UnicodeString pus = ParamStr(i);
  OutputFolder->Text=pus;
  if(wcsstr(pus.c_str(),L"-file_") != NULL)
  {
	 pus = pus.SubString(6,pus.Length());
	 OutputFolder->Text=pus; //ShowMessage(pus);
  }
 }*/
}
// ---------------------------------------------------------------------------

void __fastcall TMForm::FormDestroy(TObject *Sender)
{
 DragAcceptFiles(Handle, false);
 CIniWriter iw(L"Options.ini");
 //{ save output folder settings
	 iw.WriteString(L"mform", L"outputfolder", FolderOut.w_str());
 //}
}
// ---------------------------------------------------------------------------

void __fastcall TMForm::btn_ListClick(TObject *Sender) {
	TBitBtn* tbb = dynamic_cast<TBitBtn*>(Sender);
	if (tbb == NULL)	return;

	switch(tbb->Tag) {
	case 0:
		if (FileList->Count <= 0)			return;
		if (FileList->ItemIndex < 0)		return;
		FileList->Items->Delete(FileList->ItemIndex);
		break;
	case 1:
		if (FileList->Count <= 0)			return;
		if (FileList->ItemIndex < 0)		return;
		// if( AVG->DSSource->Opened ) return;
		if (MessageBoxW(WindowHandle,L"Вы действтельно хотите удалить выделенный файл?",L"Удаление", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1) == IDYES)
		{
			const wchar_t* wtmp = FileList->Items[0][FileList->ItemIndex].w_str();
			DeleteFileW(wtmp);
			FileList->Items->Delete(FileList->ItemIndex);
		}
		// deletefile
		break;
	case 2:
		FileList->Items->Clear();
		break;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMForm::ButtonOutFolderClick(TObject *Sender) {
	// ddd
	UnicodeString way = L"c:\\";
	if (SelectDirectory(L"Укажите путь к папке", L"", way)) {
		if (wcscmp(way.LastChar(), L"\\") != 0)	way += L'\\';
		OutputFolder->Text = way;
		FolderOut = way;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMForm::GlobalTimer(TObject *Sender) {
	unsigned int diffsframe = CountsAL - LsFrame;
	int diffframe = (int)CFrame - (int)LFrame;
	int elapseds = 0;
	int elapsedm = 0;
	int elapsedh = 0;

	if (diffframe > 0) {
		elapseds = (int)(EFrame - CFrame) / diffframe;
		elapsedh = (elapseds / 3600) % 60;
		elapsedm = (elapseds / 60) % 60;
		elapseds = elapseds % 60;
	}

	wchar_t wstat[128];
	wsprintf(wstat,	L"fcnt0( %u ) scnt( %u ), FPS(%i) SPS(%i), R(%i:%0.2im:%0.2is)",
		Counts[0], CountsAL, diffframe, diffsframe, elapsedh, elapsedm, elapseds);
	linestatus1->Text = wstat;

	if (linestatus2->Text.IsEmpty())
	{
		linestatus2->Text = AVG->DSSource->VideoCodecName + L" / " +
			AVG->DSSource->VideoFilterName + L" | " + AVG->DSSource->AudioCodecName + L" / " +
			AVG->DSSource->AudioFilterName;
	}
	// if( linestatus3->Text.IsEmpty() )
	if ((++cclocal & 7) == 0) {
		// register int sw = AVG->DSSource->Width;
		// register int sh = AVG->DSSource->Height;
		// int points = (int)(sw * sh);
		// wsprintf(wstat, L"mod4_16_64_128(%u,%u,%u,%u)WH(%ux%u)", points & 3, points & 15, points & 63, points & 127, sw, sh);
		wsprintf(wstat, L"WH(%ux%u)", AVG->DSSource->Width, AVG->DSSource->Height);
		linestatus3->Text = wstat;
	}

	if ((int)EFrame != 0)		CurrentProgressPos = (int)CFrame * 512i32 / (int)EFrame;
	if (CurrentProgressPos != LastProgressPos)		PB_AV->Position = CurrentProgressPos;
	LastProgressPos = CurrentProgressPos;
	LFrame = CFrame;
	LsFrame = CountsAL;
}
// ---------------------------------------------------------------------------

void __fastcall OpenProc(wchar_t* szPath)
{
  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));
  wchar_t fullcmd[512];

  //TEST
  wcscpy_s(fullcmd,512, L"\"C:\\Program Files\\AStepConverterX64\\StepConverter3.exe\" \"");
  wcscat_s(fullcmd,512, szPath);
  wcscat_s(fullcmd,512, L"\"");

  if ( CreateProcess(NULL, fullcmd, NULL, NULL, FALSE, CREATE_NO_WINDOW | BELOW_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi) )
  {
// программа запущена, ждем её завершения
    DWORD dwWait = WaitForSingleObject(pi.hProcess, INFINITE);
    if ( dwWait == WAIT_OBJECT_0 )
    {
// программа благополучно завершилась
    }
    else if ( dwWait == WAIT_ABANDONED )
    {
// программа была насильно "прибита"
    }
//  else ну и может быть другие варианты ожидания
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
  }
}

void __fastcall TMForm::OptionsClick(TObject *Sender) {
	TSpeedButton* tsb = dynamic_cast<TSpeedButton*>(Sender);
	if (tsb == NULL)
		return;

	//grb file
	FILE* FileOpenedGRB;
	FILE* FileHashGRB=NULL;
	UnicodeString us;
	unsigned char bbuffer[256]={0};
	wchar_t wsbuf1[512]={L'\0'};
	MMvectors mmcache;

	unsigned int numtest;
	unsigned int fileinc;
	char* massbytes=NULL;

	bool bEnabled;
	switch(tsb->Tag) {
	case 0:
		bEnabled = !AVG->FilterTransform_2->Enabled;
		SLScope1->Enabled = bEnabled;
		AVG->HistogramMain->Enabled = bEnabled;
		AVG->FilterTransform_2->Enabled = bEnabled;

		if (bEnabled) VLDSImageDisplay1->InputPin->Connect(AVG->FilterTransform_2->OutputPin);
		else
			VLDSImageDisplay1->InputPin->Disconnect();
		break;

	case 1:
		//Work on garbaje files
		if( FileList->ItemIndex == -1 ) break;
		us = FolderOut + ExtractFileName(FileList->Items[0][FileList->ItemIndex]) + L".gbin";
		FileOpenedGRB = _wfopen(FileList->Items[0][FileList->ItemIndex].w_str(),L"rb");
		if(FileOpenedGRB==NULL) break;
		FileHashGRB = (FILE*)createFile( us.w_str() );
		if(FileHashGRB != NULL)
		{
		 do
		 {
		  if( fread(bbuffer, 1, 256, FileOpenedGRB) != 256) break;
		  GroupHashWrite(bbuffer, 256, FileHashGRB, &mmcache);
		 }while(true);

		 GroupHashWrite(NULL, 0, FileHashGRB, &mmcache, true);
		 closeFile(FileHashGRB);

		}
		fclose(FileOpenedGRB);

		break;

	case 2:
		  #define maxbytes (1ui32<<20ui32)
		for(fileinc=20;fileinc<=20;++fileinc)
		for(numtest=0;numtest<20;++numtest)
		{
		 _wcsset(wsbuf1,L'\0');
		  swprintf(wsbuf1, L"%sTest(%0.2i)%0.3i.grb", FolderOut.c_str(), fileinc, numtest);

		 if(_wfopen_s(&FileHashGRB, wsbuf1, L"wb") == 0)
		 {
			setvbuf(FileHashGRB, NULL, _IOFBF, 16ui32<<20ui32 );
		 massbytes=(char*)malloc( maxbytes );
		 memset(massbytes, numtest, maxbytes);

		 for(unsigned int i=0; i<(1024*fileinc); ++i)
			fwrite(massbytes, 1, maxbytes, FileHashGRB);

		 free(massbytes);
		 fclose(FileHashGRB);

		 OpenProc(wsbuf1);
		 }
		}

		break;
	case 3:
		break;
	case 4:
		break;
	}

}
// ---------------------------------------------------------------------------