#include <stdio.h>
#include <iostream>
#include <string>

//clang++ append.cpp -O3 -o Z:\append.exe

int wmain(const int argc, const wchar_t* argv[])
{
	if(argc < 3)
	{
		std::wcerr << "help:" << std::endl;
		std::wcerr << "append file_source to_append_file" << std::endl;
		return 0;
	}
	
	std::wstring srcfn = argv[1];
	std::wstring appfn = argv[2];
		
	FILE*	wfile;
	FILE*	rfile;

	if( _wfopen_s(&rfile, srcfn.c_str(), L"rb") )	{	std::wcerr << "source file can\'t open." << std::endl; return 1;	}
	if( _wfopen_s(&wfile, appfn.c_str(), L"ab") )	{	std::wcerr << "append file can\'t open." << std::endl; return 2;	}
	
	setvbuf(rfile, nullptr, _IOFBF, 16u << 20u);
	setvbuf(wfile, nullptr, _IOFBF, 64u << 20u);
	
	alignas(16u)	unsigned char	buffer[65536u];
	size_t nread = 0;
	while( (nread = _fread_nolock_s(buffer, sizeof(buffer), 1, sizeof(buffer), rfile)) > 0u )
		_fwrite_nolock(buffer, 1, nread, wfile);
	
	fflush(wfile);
	fclose(wfile);
	fclose(rfile);
	return 0;
}