//clang++ split.cpp C:\VirtualEngine\sources\core\_ansi_\FilesManagerANSI.cpp -IC:\VirtualEngine\include -O3 -o split.exe

#include <core/FilesManager.hpp>

#include <sstream>
#include <string>

static std::wstring			openname;
static std::wstring			outdir;

int wmain(const int argc, const wchar_t* argv[])
{
	std::wstringstream	wss_converter;
	std::wstring		openfile, outdir;
	size_t				element_size = 0;
	size_t				countfiles = 0;

	if (argc <= 5)
	{
		std::wcerr << "invalid count params. -split <file name> <outdir (@ use for curdir)> <elem_size> <count_files>" << std::endl;
		return 1;
	}

	openfile = argv[2];
	outdir = argv[3];

	if (outdir.compare(L"@") == 0)
		outdir.clear();

	wss_converter.clear();
	wss_converter << argv[4];		//countfiles = static_cast<size_t>(std::wcstoull(argv[4], nullptr, 10));
	wss_converter >> countfiles;
	
	if(countfiles < 2 || countfiles > 256)
	{
		std::wcerr << "how count output files (range: 2->256)" << std::endl;
		return 1;
	}

	wss_converter.clear();
	wss_converter << argv[5]; //element_size = static_cast<size_t>(std::wcstoull(argv[5], nullptr, 10));
	wss_converter >> element_size;
	
	if(element_size < 4u || element_size > 65536u)
	{
		std::wcerr << "element size (range: 4->65536)" << std::endl;
		return 1;
	}
		
	std::vector<VX::FileUnit>	outputFiles;
				VX::FileUnit	inputFile(openname);

	if(!inputFile.OpenRead())
	{
		std::wcerr << "error: can\'t open file <" << openname << '>' << std::endl;
		return 1;
	}
		
	// std::wstring			writename;
	
	// for(unsigned int i=0u; i<32u; ++i)
	// {
		// writename.clear();
		
		// writename = outdir + L'\\';		
		// writename += L"GHI_32_";
		// if(i<10)
			// writename += '0';
		// writename += std::to_wstring(i);
		// writename += L".story";
		
		// if( _wfopen_s(&wfile[i], writename.c_str(), L"ab") )	return 2 + i;
		
		// setvbuf(wfile[i], nullptr, _IOFBF, 512u << 20u);
	// }

	// alignas(16u)	unsigned int	buffer[4u];
	// size_t nread;
	// while( (nread = _fread_nolock_s(buffer, sizeof(buffer), 1, sizeof(buffer), hfile)) == sizeof(buffer) )
	// {
		// unsigned int ind = buffer[0u] & 31u;
		// _fwrite_nolock(buffer, 1, sizeof(buffer), wfile[ind]);
	// }
	
	// for(unsigned int i=0u; i<32u; ++i)	
	// {
		// fflush(wfile[i]);
		// fclose(wfile[i]);
	// }
	
	// fclose(hfile);
	// return 0;
}