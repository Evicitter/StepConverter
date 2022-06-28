//clang++ split.cpp C:\VirtualEngine\sources\core\_ansi_\FilesANSI.cpp -IC:\VirtualEngine\include -O3 -o split.exe
//clang++ split.cpp C:\VirtualEngine\sources\core\_win_\FilesWindows.cpp -IC:\VirtualEngine\include -O3 -o split.exe
#include <core/Files.hpp>

#include <sstream>
#include <string>
#include <memory>

int wmain(const int argc, const wchar_t* argv[])
{
	std::wstringstream	wss_converter;
	std::wstring		openname;
	std::wstring		outdir;
	std::wstring		writename;
	size_t				element_size = 0;
	size_t				countfiles = 0;

	if (argc < 5)
	{
		std::wcerr << "invalid count params. -split <file name> <outdir (@ use for curdir)> <elem_size> <count_files>" << std::endl;
		return 1;
	}

	openname	= argv[1];
	outdir 		= argv[2];

	if ((outdir.size() > 3u) && (outdir.back() != L'\\' && outdir.back() != L'/'))
		outdir += L'\\';
	
	if (outdir.compare(L"@") == 0)
		outdir.clear();

	std::wistringstream(argv[3]) >> element_size;
	if(element_size < 4u || element_size > 65536u)
	{
		std::wcerr << "element size (range: 4->65536)" << std::endl;
		return 1;
	}
	
	std::wistringstream(argv[4]) >> countfiles;
	if(countfiles < 2 || countfiles > 256)
	{
		std::wcerr << "how count output files (range: 2->256)" << std::endl;
		return 1;
	}	
	
	VX::FileUnit								inputFile(openname);
	std::vector<std::unique_ptr<VX::FileUnit>>	outputFiles;
	
	if(!inputFile.OpenRead())
	{
		std::wcerr << "error: can\'t open file <" << openname << '>' << std::endl;
		return 1;
	}
	
	writename.reserve(32u);
	outputFiles.reserve(countfiles);
	
	for(unsigned int i=0u; i<countfiles; ++i)
	{
		writename.clear();
		if(!outdir.empty())
			writename = outdir + L"\\";
		writename += L"SPLIT_";
		if(i<100)	writename += L'0';
		if(i<10)	writename += L'0';
		writename += std::to_wstring(i);
		writename += L".story";
		outputFiles.emplace_back( std::make_unique<VX::FileUnit>(writename) );
		if(outputFiles.back()->OpenWrite())
		{
			//outputFiles.back()->Prealloc();
			outputFiles.back()->setDeleteOnClose(true);
		}
		else
		{
			std::wcerr << "error: can\'t write file <" << writename << '>' << std::endl;
			return 1;
		}
	}

	alignas(16u)	unsigned char	buffer[65536u];
	size_t							nread;
	while( (nread = inputFile.Read(buffer, element_size)) == element_size )
	{
		unsigned int ind = static_cast<unsigned int>(buffer[0u]) % countfiles;
		outputFiles[ind]->Write(buffer, element_size);
	}
	inputFile.Close();
	
	for(auto& ofile : outputFiles)
	{
		ofile->Flush();
		ofile->setDeleteOnClose(false);
		ofile->Close();
	}
	
	return 0;
}