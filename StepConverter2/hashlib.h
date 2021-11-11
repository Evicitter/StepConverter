//Last update time : 23.07.2013 22:24:42
#pragma once

#ifdef HASHLIB_EXPORTS
#define HASHLIB_API(type) __declspec(dllexport) ##type __fastcall
#else
 #ifdef __BORLANDC__
  #define HASHLIB_API(type) __declspec(dllimport) ##type __msfastcall
 #else
  #define HASHLIB_API(type) __declspec(dllimport) ##type __fastcall
 #endif
#endif

#include "m_typedef.h"

#ifndef MAKE8CC
#define MAKE8CC(a,b,c,d,e,f,g,h) ((__int64)h<<56) | ((__int64)g<<48) | ((__int64)f<<40) | ((__int64)e<<32) | ((__int64)d<<24) | ((__int64)c<<16) | ((__int64)b<<8) | ((__int64)a)
#endif

extern "C" HASHLIB_API(int) fnhashlib(int test);

//File Hash
extern "C" HASHLIB_API (void) mmxMD5_s( md5_context* ctx );
extern "C" HASHLIB_API (void) mmxMD5_u( md5_context* ctx, const void* buf, unsigned int len);
extern "C" HASHLIB_API (void) mmxMD5_f( md5_context* ctx, unsigned char* output);	//output - 128bit

extern "C" HASHLIB_API (void) mmxRHash_s( rhash_context* rhctx ); //state - 128bit
extern "C" HASHLIB_API (void) mmxRHash_u( rhash_context* rhctx, const void* buf, unsigned int len);
extern "C" HASHLIB_API (void) mmxRHash_f( rhash_context* rhctx, unsigned char* output); //output - 128bit

extern "C" HASHLIB_API (void) mmxRHash64_s( rhash64_context* rhctx ); //state - 128bit
extern "C" HASHLIB_API (void) mmxRHash64_u( rhash64_context* rhctx, const void* buf, unsigned int len);
extern "C" HASHLIB_API (void) mmxRHash64_f( rhash64_context* rhctx, unsigned char* output); //output - 128bit

extern "C" HASHLIB_API (void) mmxAES_complex_s( aes_context_complex* aesctx );
extern "C" HASHLIB_API (void) mmxAES_complex_u( aes_context_complex* aesctx, const void* buf, unsigned int len);
extern "C" HASHLIB_API (void) mmxAES_complex_f( aes_context_complex* aesctx ); //output - NULL - out values in context

extern "C" HASHLIB_API (void) mmxAES_DM_s( aes_context* aesctx );
extern "C" HASHLIB_API (void) mmxAES_DM_u( aes_context* aesctx, const void* buf, unsigned int len);
extern "C" HASHLIB_API (void) mmxAES_DM_f( aes_context* aesctx ); //output - NULL - out values in context
//--------------------------------------------------------------------------------------

//File Map
extern "C" HASHLIB_API(void) getDiffColor(const void* rgba, unsigned int len_in, int h, int w, float* outval); //outval - 256 * 256 * sizeof(double)
//--------------------------------------------------------------------------------------

//File Work ----------------------------------------------------------------------------
extern "C" HASHLIB_API(void*) createFile(wchar_t* fname, bool bTemp = false); //return FILE*
extern "C" HASHLIB_API(void*) openFile(wchar_t* fname);						 //return FILE*
extern "C" HASHLIB_API(int) writeFile(void* fileno, void* buf, __int64 fmt, __int64 count, char size, __int64 index);
extern "C" HASHLIB_API(int) readFile(void* fileno, void* buf, __int64& fmt, __int64& count, char& size, __int64& index);
extern "C" HASHLIB_API(int) closeFile(void* fileno);

extern "C" HASHLIB_API(int) beginHeaderFile(void* fileno, __int64 fmt, __int64 count, char size, __int64 index, short groups);
extern "C" HASHLIB_API(int) writeChunkFile(void* fileno, void* buf, __int64 count, char size);

extern "C" HASHLIB_API(int) writeChunkElement(void* fileno, __int64 fmt_header, unsigned int count, unsigned short size, char* buffer); //fmt_header - MAKEEIGTCC('','',...,'');
//--------------------------------------------------------------------------------------