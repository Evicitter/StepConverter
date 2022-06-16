#include <stdio.h>
#include <io.h>
//#include <stdlib.h>
#include <iostream>
#include <string>

//clang++ back_split_truncate.cpp -O3 -mtune=nehalem -march=nehalem -o back_split_truncate.exe

int main()
{
	std::wstring			openname;
	std::wstring			outdir;

	std::wcerr << "open file name = ";
	std::wcin >> openname;
	std::wcerr << "openname = " << openname << std::endl;

	std::wcerr << "save directory (\'@\' as current directory = ";
	std::wcin >> outdir;
	if(!outdir.compare(L"@"))
	{
		outdir.clear();
		std::wcerr << "outdir = <current>" << std::endl;
	}
	else
		std::wcerr << "outdir = " << outdir << std::endl;
	
	FILE*	wfile[256u];
	FILE*	hfile;
	
	std::wstring			writename;
	
	for(unsigned int i=0u; i<256u; ++i)
	{
		writename.clear();
		
		if(!outdir.empty())
			writename = outdir + L'\\';
		
		writename += L"GHI_32_";
		
		if	(i<100)		writename += L'0';
		if	(i<10)		writename += L'0';
		
		writename += std::to_wstring(i);
		writename += L".story";
		
		if( _wfopen_s(&wfile[i], writename.c_str(), L"ab") )	return 2 + i;
		
		setvbuf(wfile[i], nullptr, _IOFBF, 64u << 20u);
	}
	
	if( _wfopen_s(&hfile, openname.c_str(), L"r+b") )	return 1;
	
	do
	{
		_fseeki64_nolock(hfile, 0, SEEK_END);
		
		long long filesize = _ftelli64_nolock(hfile);
		
		//if(filesize <= (2u << 30u))	break;
		if(filesize <= 0)	break;
		
		//if(_fseeki64_nolock(hfile, -8'589'934'592ll, SEEK_CUR))
		if(_fseeki64_nolock(hfile, -1'073'741'824ll, SEEK_CUR))
		   _fseeki64_nolock(hfile, 0, SEEK_SET);
		
		long long truncatepos = _ftelli64_nolock(hfile);
		
		std::wcerr << "filesize:truncpos = " << filesize << ':' << truncatepos << std::endl;
		
		//std::wcin.ignore();
		
		alignas(16u)	unsigned int	buffer[4u];
		size_t nread;
		while( (nread = _fread_nolock_s(buffer, sizeof(buffer), 1, sizeof(buffer), hfile)) == sizeof(buffer) )
		{
			unsigned int ind = buffer[0u] & 255u;
			_fwrite_nolock(buffer, 1, sizeof(buffer), wfile[ind]);
		}
		
		switch(_chsize_s(_fileno(hfile), truncatepos))
		{
			case 0:			std::wcerr << "_chsize trunc succ\n";	break;
			case EACCES:	std::wcerr << "_chsize ACCES\n";		break;
			case EBADF:		std::wcerr << "_chsize EBADF\n";		break;
			default:		std::wcerr << "_chsize other error\n";	break;
		}
	}while(true);
	
	fclose(hfile);
	
	for(unsigned int i=0u; i<256u; ++i)
	{
		fflush(wfile[i]);
		fclose(wfile[i]);
	}
	return 0;
}