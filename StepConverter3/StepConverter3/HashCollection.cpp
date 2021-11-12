//Last update time : 23.10.2013 12:31:11
#include "HashCollection.h"
#include "HashClasses.h"

int ind_rhash;
int ind_rhash64;
int ind_aesdm;
int ind_aeshi;

void __fastcall PushHash(binFile* bfw, const unsigned char* inbuf, const unsigned long long inlen, const bool binit)
{
	if(bfw==NULL) return;

	if(binit)
	{
		ind_rhash	= bfw->initChunkElement(*(unsigned long long*)"RHASH",	16ui16, 524288);
		//ind_rhash64 = bfw->initChunkElement(*(unsigned long long*)"RHASH64", 8ui16, 524288);
		ind_aesdm	= bfw->initChunkElement(*(unsigned long long*)"AES_DM", 32ui16, 262144);
		//ind_aeshi	= bfw->initChunkElement(*(unsigned long long*)"AES_HI", 32ui16, 131072);
	 return;
	}

	unsigned char chash[32];

	RHash gRH;
	gRH.Start();
	gRH.Update(inbuf, inlen);
	gRH.Finish(chash);

	bfw->pushChunkElement(ind_rhash, chash);
	//*(unsigned __int64*)chash += *((unsigned __int64*)chash + 1);
	//bfw->pushChunkElement(ind_rhash64, chash);

	AESDM gAESDM;
	gAESDM.Start();
	gAESDM.Update(inbuf, inlen);
	gAESDM.Finish(chash);

	bfw->pushChunkElement(ind_aesdm, chash);
	//bfw->pushChunkElement(ind_aeshi, chash);
}

void __fastcall getGRB(const wchar_t* fname, const wchar_t* woutfilename, binFile* bfw, const int blocksize)
{
	if(bfw==NULL) return;

	FILE* bFile=NULL;
	if(_wfopen_s(&bFile, fname, L"rbS")==0)
	{
		  bfw->saveFile(woutfilename);

		  setvbuf(bFile, NULL, _IOFBF, 4<<20);
		  
		  const size_t readBlock=blocksize;

		  PushHash(bfw, NULL, 0, true);

#pragma omp parallel //firstprivate(readBlock)
		 {
		  size_t readB=0;
		  
		  unsigned char* bbuf = (unsigned char*)_aligned_malloc(readBlock,16);
		  memset(bbuf,255,readBlock);
		  do
		  {
			  readB = fread_s(bbuf, readBlock, 1, readBlock, bFile); //_fread_nolock_s(bbuf, readBlock,1,readBlock,bFile);
			  if(readB>0 && readB!=readBlock) memset(bbuf+readB,255,readBlock-readB);
			  if(readB>0)
				PushHash(bfw, bbuf, readBlock);
		  }while(readB==readBlock);
		  _aligned_free(bbuf);
		 }
		  fclose(bFile);
		  
		  bfw->closeFile();
	}
}