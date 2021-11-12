//Last update time : 27.09.2013 22:53:28
#pragma once

#include <stdio.h>
#include <vector>

struct sChunks
{
	void*		 buffer;
	size_t real_inbuf;
	size_t index_inbuf;

	unsigned int maxcountcache;
	unsigned int count;
	unsigned short size;
	__int64	 fmtheader;

	sChunks()
	{
		buffer=NULL;
		index_inbuf=real_inbuf=0;
		count=0;
		size=1;
		fmtheader=0;
	}
};

class binFile
{
	FILE* hFile;
	int isOpenFile;
	int crecords;

	std::vector<sChunks> cacherec;

public:

	binFile(void);
	~binFile(void);

	int openFile(const wchar_t* fname);
	int saveFile(const wchar_t* fname);
	int closeFile();
	int setpos( __int64 pos );
__int64 getpos(  );

	int checkRecords();

	//read
	int getCurFmt( __int64& fmt, __int64& count, unsigned char& size, short& groups, wchar_t** name );
	int skipCurFmt( );
	int getCurElement( __int64& fmt_header, unsigned char& size);
	int skipCurElement( );

	//get
	int setArray( __int64  fmt, __int64  count, unsigned char  size, short  groups, wchar_t*  name, void* Buf );
	void* getArray( __int64& fmt, __int64& count, unsigned char& size, short& groups, wchar_t** name, void* Buf, bool bNextChunk=true );
	int FreeArray( void* Buf );

	void* getElement( __int64& fmt_header, unsigned char& size, void* Buf, bool bNextChunk=true);
	long long GetCollide( __int64 fmt_header, unsigned char size );
  //------------------------------------------------------------------------------------------------

	//writecahce --------------------------------------------------------------------------------------
	int  initChunkElement(__int64 fmt_header, unsigned short size, unsigned int maxcountcache); //return index
	int  pushChunkElement(int index, unsigned char* buff);

	int  writeChunkElement(__int64 fmt_header, unsigned int count, unsigned short size, const void* buffer);
	//-------------------------------------------------------------------------------------------------

	//write
	int beginHeaderFile( __int64 fmt, __int64 count, char size, short groups, wchar_t* name );
	int writeChunk( void* buf, __int64 count, char size );

	//find

	//---------------------------------------------------------------------------------------------

	//export
	int exportText(wchar_t* filename);
	//---------------------------------------------------------------------------------------------
};