//Last update time : 27.09.2013 22:22:28
#pragma once

#include "binFile.h"
#include "HashClasses.h"

extern int ind_rhash;
extern int ind_rhash64;
extern int ind_aesdm;
extern int ind_aeshi;

extern void __fastcall PushHash(binFile* bfw, const unsigned char* inbuf, const unsigned long long inlen, const bool binit=false);
extern void __fastcall getGRB(const wchar_t* fname, const wchar_t* woutfilename, binFile* bfw, const int blocksize);