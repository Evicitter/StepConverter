//Last update time : 01.10.2012 21:57:51
//#include <CVLGenericFilter.h>

#include "stdafx.h"
#include <Windows.h>

#include <intrin.h>

//global variables
//extern FILE* cfVideo_F06;
//extern FILE* cfVideo_F09;
//extern FILE* cfVideo_F15;

extern unsigned int cPixel;
extern unsigned int cdSize;
extern unsigned int ciSize;

extern unsigned long* vhg06;  //aligned
extern unsigned long* _VHG06; //aligned
extern float* glF06;  //aligned
extern unsigned long glSpectMax;

//Funct init files -----------------------------------------------------------
extern void exSetFiles_Init(bool bOncefld, const wchar_t* mediafile);
extern void exSetFiles_Close();
extern void exSetFiles_Flush(long param1);
//---------------------------------------------------------------------------

//Functions ---------------------------------------------------------------------
extern void __fastcall create_rend(int w, int h);
extern void __fastcall destroy_rend();

extern void __fastcall init_rend(const unsigned char* ptr);

extern void __fastcall rend_F06( const unsigned int p, unsigned int NumThread ); //one arg;
extern void __fastcall rend_F09( const unsigned int p, unsigned int NumThread ); //one arg;
extern void __fastcall rend_F15( const unsigned int p, unsigned int NumThread ); //one arg;
extern void __fastcall rend_F24( const unsigned int p, unsigned int NumThread ); //one arg;
extern void __fastcall rend_F27( const unsigned int p, unsigned int NumThread ); //one arg;
//-------------------------------------------------------------------------------

extern __m128i hash(char* buffer, size_t buflen);

extern unsigned int __fastcall xRGB2H(__m128i bgr); //(unsigned char r, unsigned char g, unsigned char b);
extern __m128i __fastcall xRGB2xHSV(__m128i bgr); //(unsigned char r, unsigned char g, unsigned char b);
