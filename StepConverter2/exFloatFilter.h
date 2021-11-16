//Last update time : 21.07.2010 21:20:04
//#include <CVLGenericFilter.h>

#include "stdafx.h"
#include <Windows.h>

//global variables
extern unsigned long VidHistGeneric06[256];
extern unsigned long VidHistGeneric09[256];

extern CFile cfVideo_F06;
extern CFile cfVideo_F09;
extern CFile cfVideo_F15;

//Funct init files -----------------------------------------------------------

extern void exSetFiles_Init(bool bOncefld, CString* workfld, CString* mediafile);
extern void exSetFiles_Close();
extern void exSetFiles_Flush();

//---------------------------------------------------------------------------

//OpenCL ------------------------------------------------------------------
extern void Cleanup(int iExitCode);
extern void CleanupKernel(int iExitCode);
extern void CleanupMem(int iExitCode);

extern int __fastcall exFF_Initial(HWND hw); //return error - 0 = success
extern int __fastcall exFF_Compile(HWND hw); //return error - 0 = success
extern int __fastcall exFF_Kernel_1(HWND hw); //return error - 0 = success

extern int __fastcall exFF_CreateBuf_1(HWND hw, int w, int h); //return error - 0 = success
extern int __fastcall exFF_FreeBuf_1(HWND hw); //return error - 0 = success

extern int __fastcall exFF_execK1(HWND hw, const unsigned char* p); //return error - 0 = success
//----------------------------------------------------------------------------