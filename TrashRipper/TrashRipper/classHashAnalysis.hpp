//-----------------------------------------------
//	Create				:	03.11.2021 15:03:06
//
//	Last update time	:	24.06.2022 21:57:08
//-----------------------------------------------
//	[PROTOTYPE]
//-----------------------------------------------
#pragma once

#include <time.h>
#include <iostream>
#include <string>

#include <concurrent_unordered_set.h>

#include <thread>
#include <mutex>
#include <nmmintrin.h>

#include <core/hashes/ColidPool.hpp>

namespace strategies
{
	class _FileProcessingStrategyBase
	{
	protected:
		uint64_t			collisions;
		uint64_t			elements_count;
		clock_t				iteration_clk;
		clock_t				summary_clk;

	public:
		_FileProcessingStrategyBase() : collisions(0ull), elements_count(0ull), iteration_clk(0),	summary_clk(0)	{}
		virtual ~_FileProcessingStrategyBase() {}
		
		virtual bool check(FILE*	hashFile,							std::wostream& out)	=	0;
		virtual bool flatten(FILE*	hashFile,	FILE*	hashOutFile,	std::wostream& out)	=	0;

		virtual bool setfilesize( const unsigned long long _filesize )	{ return false; }

		uint64_t	getColsCounts()	const	{	return collisions;		}
		uint64_t	getElemCounts()	const	{	return elements_count;	}
	};

	template<const unsigned int TSIZE>
	class _SimpleFile : public _FileProcessingStrategyBase
	{
	protected:
		typedef std::array<unsigned char, TSIZE>				mytype;
		//std::unordered_set< mytype >							xhashtable;
		concurrency::concurrent_unordered_set< mytype >			xhashtable;

	public:
		~_SimpleFile()	override		{
			xhashtable.clear();
		}

		bool	setfilesize(const unsigned long long _filesize)	override
		{
			size_t	ressize = (_filesize <= (1ull << 30u)) ? _filesize : (1ull << 30u);
					ressize /= TSIZE;
			xhashtable.rehash(ressize);	//xhashtable.reserve( ressize );
			return true;
		}

		bool	check(FILE*		hashInFile,							std::wostream& out)	override;
		bool	flatten(FILE*	hashInFile,	FILE*	hashOutFile,	std::wostream& out)	override;
	};

	template<const unsigned int TSIZE>
	class _VeryBigFile : public _FileProcessingStrategyBase
	{
	public:
	protected:
		typedef std::array<unsigned char, TSIZE>		mytype;
		VX::hash::XHashTableLimit< mytype >				xhashtable;

	public:
		~_VeryBigFile()	override {
			xhashtable.clear();
		}

		bool	check(FILE*		hashInFile, std::wostream& out)	override;
		bool	flatten(FILE*	hashInFile, FILE*	hashOutFile, std::wostream& out)	override;
	};


	class ConfigReader
	{
	public:
		ConfigReader() {}

		unsigned long long	getValue(const char* valuename, unsigned long long _default = 0ull)
		{
			std::ifstream				fileconfig("main.ini");
			unsigned long long			returnv = _default;
			if (fileconfig.is_open())
			{
				std::string	rvaluename;
				unsigned long long	readv;

				while (!fileconfig.eof())
				{
					fileconfig >> rvaluename;
					if (rvaluename.compare(valuename))
					{
						fileconfig >> rvaluename;
						fileconfig >> readv;
						continue;
					}
					else
					{
						fileconfig >> rvaluename;
						fileconfig >> readv;
						returnv = readv;
						break;
					}
				}
			}
			return returnv;
		}
	};
};

class HashAnalysis
{
	std::vector< std::tuple<std::wstring, uint64_t, uint64_t> > results;
	std::unique_ptr< strategies::_FileProcessingStrategyBase >	strategies;
	FILE*		h_inHashFile;
	FILE*		h_outHashFile;
	long long	filesize;
	clock_t		total_clocks;

protected:
	bool	OpenFile(const std::wstring& ffilename)
	{
		if (_wfopen_s(&h_inHashFile, ffilename.c_str(), L"rbS"))
			return false;

		//check file size	----------------------------
		_fseeki64_nolock(h_inHashFile, 0, SEEK_END);
		filesize = _ftelli64_nolock(h_inHashFile);
		_fseeki64_nolock(h_inHashFile, 0, SEEK_SET);
		//----------------------------------------------

		setvbuf(h_inHashFile, nullptr, _IOFBF, 8u << 20u);

		return true;
	}
	bool	CloseFile()
	{
		return fclose(h_inHashFile) == 0;
	}

public:
	HashAnalysis() : h_inHashFile(nullptr), h_outHashFile(nullptr), filesize(0ll), total_clocks(0) {}

	void	dump(std::wostream&	out)	const {
		out	<<	"summary clock = "	<<	(total_clocks / CLOCKS_PER_SEC)	<<	's'	<<	std::endl;
	}

	void print(std::wostream& out)		const {
		if(results.empty())
			return;

		out << L"╔════════════════════════════════╦════════════════╦════════════════╗" << std::endl;
		out << L"║              name              ║   collisions   ║    elements    ║" << std::endl;
		out << L"╠════════════════════════════════╬════════════════╬════════════════╣" << std::endl;
		uint64_t sumcols = 0u, sumelems = 0u;
		for(const auto& it : results)
		{
			out << L"║";
			out.width(32u);	out << std::get<0u>(it); out << L"║";
			out.width(16u); out << std::get<1u>(it); out << L"║";
			out.width(16u); out << std::get<2u>(it); out << L"║";
			out << std::endl;
			sumcols  += std::get<1u>(it);
			sumelems += std::get<2u>(it);
		}
		out << L"╠════════════════════════════════╬════════════════╬════════════════╣" << std::endl;
		out << L"║";
					out.width(32u);	out << "<summary>";	out << L"║";
					out.width(16u); out << sumcols;		out << L"║";
					out.width(16u); out << sumelems;	out << L"║";
					out << std::endl;
		out << L"╚════════════════════════════════╩════════════════╩════════════════╝" << std::endl;
		//out << L"╔═╦╗";
		//out << L"╠═╬╣";
		//out << L"║ ║║";
		//out << L"╚═╩╝";
	}

	bool	check(const wchar_t* hname, const unsigned int hsize, const std::wstring& fromdir, std::wostream& out)
	{
		clock_t	function_clock	=	clock();
		const std::wstring ffilename = fromdir + hname;
		
		if(!OpenFile(ffilename))
		{
			std::wcerr << "File \'" << ffilename << "\' don\'t opened.\n";
			return false;
		}

		//Select strategy	--------------------------------------------------------
		//0 - simple
		//1 - bigfile
		//2 - verybigfile
		if(hsize == 16)
		{
			//if		(filesize <= (512ll << 20u))
			//	strategies = std::make_unique< strategies::_SimpleFile<16u> >();
			//else
				strategies = std::make_unique< strategies::_VeryBigFile<16u> >();
		}
		else if (hsize == 32)
		{
			//if (filesize <= (1ll << 30u))
			//	strategies = std::make_unique< strategies::_SimpleFile<32u> >();
			//else
				strategies = std::make_unique< strategies::_VeryBigFile<32u> >();
		}
		//--------------------------------------------------------------------------

		bool	bresult	=	false;
		if(strategies)
		{
				out << hname << std::endl;
							strategies->setfilesize(filesize);
				bresult	=	strategies->check(h_inHashFile, out);
				
				results.emplace_back( std::make_tuple(hname, strategies->getColsCounts(), strategies->getElemCounts()) );
		}
		CloseFile();
		function_clock	=	clock()	- function_clock;
		total_clocks	+=	function_clock;
		return bresult;
	}

	bool	flatten(const wchar_t* hname, const unsigned int hsize, const std::wstring& fromdir, std::wostream& out)
	{
		const std::wstring ffilename = fromdir + hname;
		const std::wstring ffilenameW = fromdir + L"GHI_TEMPTEMP.story";

		if (!OpenFile(ffilename))
		{
			std::wcerr << "File \'" << ffilename << "\' don\'t opened.\n";
			return false;
		}

		if (_wfopen_s(&h_outHashFile, ffilenameW.c_str(), L"wbS"))
		{
			std::wcerr << "File \'" << ffilenameW << "\' don\'t save.\n";
			return false;
		}

		setvbuf(h_outHashFile, nullptr, _IOFBF, 1u << 20u);

		//Select strategy
		//0 - simple
		//1 - bigfile
		//2 - verybigfile

		if (hsize == 16)
		{
			//if (filesize <= (1ll << 30u))
			//	strategies = std::make_unique< strategies::_SimpleFile<16u> >();
			//else
				strategies = std::make_unique< strategies::_VeryBigFile<16u> >();
		}
		else if (hsize == 32)
		{
			//if (filesize <= (2ll << 30u))
			//	strategies = std::make_unique< strategies::_SimpleFile<32u> >();
			//else
				strategies = std::make_unique< strategies::_VeryBigFile<32u> >();
		}
		//----------------------------------------------

		bool	bresult = false;
		if (strategies)
		{
			out << hname << std::endl;
			strategies->setfilesize(filesize);
			bresult = strategies->flatten(h_inHashFile, h_outHashFile, out);

			results.emplace_back(std::make_tuple(hname, strategies->getColsCounts(), strategies->getElemCounts()));
		}

		CloseFile();
		fclose(h_outHashFile);

		_wremove(ffilename.c_str());
		_wrename(ffilenameW.c_str(), ffilename.c_str());

		return bresult;
	}
};

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//          ███████╗██╗███╗   ███╗██████╗ ██╗     ███████╗███████╗██╗██╗     ███████╗
//          ██╔════╝██║████╗ ████║██╔══██╗██║     ██╔════╝██╔════╝██║██║     ██╔════╝
//          ███████╗██║██╔████╔██║██████╔╝██║     █████╗  █████╗  ██║██║     █████╗  
//          ╚════██║██║██║╚██╔╝██║██╔═══╝ ██║     ██╔══╝  ██╔══╝  ██║██║     ██╔══╝  
//  ███████╗███████║██║██║ ╚═╝ ██║██║     ███████╗███████╗██║     ██║███████╗███████╗
//  ╚══════╝╚══════╝╚═╝╚═╝     ╚═╝╚═╝     ╚══════╝╚══════╝╚═╝     ╚═╝╚══════╝╚══════╝
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
template<const unsigned int TSIZE>
bool	strategies::_SimpleFile<TSIZE>::check(FILE*	hashInFile,	std::wostream& out)
{
	summary_clk = clock();

	std::vector<std::unique_ptr<std::thread>> Threads;
	for (unsigned int i = std::thread::hardware_concurrency(); i > 0; --i)
		Threads.push_back(std::move(std::unique_ptr<std::thread>(new std::thread(
			[&]()
	{
		alignas(32u)	mytype	elem;
		size_t nread;
		decltype(collisions)		tcollisions		= decltype(collisions)();
		decltype(elements_count)	telements_count = decltype(elements_count)();

		while ((nread = fread_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size())
		{
			if (xhashtable.insert(elem).second);
				//ms.SaturateSpin(elem.data(), elem.size());
			else
				++tcollisions;
			++telements_count;
		}
		_InterlockedAdd64(reinterpret_cast<volatile long long*>(&collisions),		static_cast<long long>(tcollisions));
		_InterlockedAdd64(reinterpret_cast<volatile long long*>(&elements_count),	static_cast<long long>(telements_count));
	}))));

	for (auto& it : Threads) it->join();
	Threads.clear();

	summary_clk = clock() - summary_clk;

	out << "\n\tcollisions     : " << collisions;
	out << "\n\telements_count : " << elements_count;
	out << "\n\ttime work      : " << (summary_clk / CLOCKS_PER_SEC) << 's';

	//out << "\n\tsize       : " << xhashtable.size();
	//out << "\n\tbucks      : " << xhashtable.bucket_count();
	//out << "\n\tlfactor    : " << xhashtable.load_factor();
	//out << "\n\tmfactor    : " << xhashtable.max_load_factor();

	//out << "\nMaterialSpector: dump\n";
	//ms.dump(out);
	out << '\n';
	return true;
}

template<const unsigned int TSIZE>
bool	strategies::_SimpleFile<TSIZE>::flatten(FILE*	hashInFile,	FILE*	hashOutFile,	std::wostream& out)
{
	summary_clk = clock();

	std::vector<std::unique_ptr<std::thread>> Threads;
	for (unsigned int i = std::thread::hardware_concurrency(); i > 0; --i)
		Threads.push_back(std::move(std::unique_ptr<std::thread>(new std::thread(
			[&]()
	{
		alignas(32u)	mytype	elem;
		size_t nread;
		decltype(collisions)		tcollisions		= decltype(collisions)();
		decltype(elements_count)	telements_count = decltype(elements_count)();

		while ((nread = fread_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size())
		{
			if (xhashtable.insert(elem).second);
			//ms.SaturateSpin(elem.data(), elem.size());
			else
				++tcollisions;
			++telements_count;
		}
		_InterlockedAdd64(reinterpret_cast<volatile long long*>(&collisions),		static_cast<long long>(tcollisions));
		_InterlockedAdd64(reinterpret_cast<volatile long long*>(&elements_count),	static_cast<long long>(telements_count));
	}))));

	for (auto& it : Threads) it->join();
	Threads.clear();

	summary_clk = clock() - summary_clk;

	out << "\n\tcollisions     : " << collisions;
	out << "\n\telements_count : " << elements_count;
	out << "\n\ttime work      : " << (summary_clk / CLOCKS_PER_SEC) << 's';

	//out << "\nMaterialSpector: dump\n";
	//ms.dump(out);
	out << '\n';

	//save
	for (const auto& idata : xhashtable)
		_fwrite_nolock((char*)&idata.at(0), 1u, idata.size(), hashOutFile);

	return true;
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//         ██╗   ██╗███████╗██████╗ ██╗   ██╗██████╗ ██╗ ██████╗ ███████╗██╗██╗     ███████╗
//         ██║   ██║██╔════╝██╔══██╗╚██╗ ██╔╝██╔══██╗██║██╔════╝ ██╔════╝██║██║     ██╔════╝
//         ██║   ██║█████╗  ██████╔╝ ╚████╔╝ ██████╔╝██║██║  ███╗█████╗  ██║██║     █████╗  
//         ╚██╗ ██╔╝██╔══╝  ██╔══██╗  ╚██╔╝  ██╔══██╗██║██║   ██║██╔══╝  ██║██║     ██╔══╝  
//  ███████╗╚████╔╝ ███████╗██║  ██║   ██║   ██████╔╝██║╚██████╔╝██║     ██║███████╗███████╗
//  ╚══════╝ ╚═══╝  ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═════╝ ╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝╚══════╝                                                                                      
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
template<const unsigned int TSIZE>
bool	strategies::_VeryBigFile<TSIZE>::check(FILE*	hashInFile, std::wostream& out)
{
	std::vector<std::unique_ptr<std::thread>> Threads;
	Threads.reserve(4u);

	iteration_clk = clock();
	{
		mytype	elem;
		size_t nread;
		//_fseeki64_nolock(hashInFile, 0ll, SEEK_SET);
		rewind(hashInFile);
		while ((nread = _fread_nolock_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size())
			xhashtable.Pass0_BuildHistogram(elem);
		//xhashtable.loadHistogram();
	}
	iteration_clk = clock() - iteration_clk;
	summary_clk	+= iteration_clk;

	std::wcerr << "Time histogram build : " << (iteration_clk / CLOCKS_PER_SEC) << 's' << std::endl;

	ConfigReader	ConfigEx;

	for (xhashtable.beginLimiters(); xhashtable.endLimiters(ConfigEx.getValue("startLimitGB", 24ull) << 30u); xhashtable.incLimiters())
	{
		decltype(collisions)		lcollisions		= decltype(collisions)();
		decltype(elements_count)	lelements_count = decltype(elements_count)();

		std::wcerr << ": proccessed at index : "; //<< NINDEX;
		xhashtable.print_status_range(std::wcerr);

		//_fseeki64_nolock(hashInFile, 0ll, SEEK_SET);
		rewind(hashInFile);

		iteration_clk = clock();

		for (unsigned int i = std::thread::hardware_concurrency(); i > 0; --i)
			Threads.push_back(std::move(std::unique_ptr<std::thread>(new std::thread(
				[&]()
		{
			alignas(32u)	mytype elem;
			size_t nread;
			decltype(collisions)		tcollisions		= decltype(collisions)();
			decltype(elements_count)	telements_count = decltype(elements_count)();

			while (	(nread = fread_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size()	)
			{
				auto resins = xhashtable.insert(elem);
				if (resins.first)
				{
					if (resins.second);
						//ms.SaturateSpin(elem.data(), elem.size());
					else
						++tcollisions;
					++telements_count;
				}
			}
			_InterlockedAdd64(reinterpret_cast<volatile long long*>(&lcollisions),		static_cast<long long>(tcollisions));
			_InterlockedAdd64(reinterpret_cast<volatile long long*>(&lelements_count),	static_cast<long long>(telements_count));
		}))));

		for (auto& it : Threads)	it->join();
		Threads.clear();

		iteration_clk = clock() - iteration_clk;
		summary_clk += iteration_clk;

		collisions		+=	lcollisions;
		elements_count	+=	lelements_count;

		std::wcerr << "\tclock(" << iteration_clk << "ms) cols(" << lcollisions << ") count(" << lelements_count << ')' << std::endl;
	}

	out << "\n\tcollisions     : " << collisions;
	out << "\n\telements_count : " << elements_count;
	out << "\n\ttime work      : " << (summary_clk / CLOCKS_PER_SEC) << 's';

	//out << "\nMaterialSpector: dump\n";
	//ms.dump(out);
	out << '\n';
	return true;
}

template<const unsigned int TSIZE>
bool	strategies::_VeryBigFile<TSIZE>::flatten(FILE*	hashInFile, FILE*	hashOutFile, std::wostream& out)
{
	std::vector<std::unique_ptr<std::thread>> Threads;
	Threads.reserve(4u);

	iteration_clk = clock();
	{
		mytype	elem;
		size_t nread;
		//_fseeki64_nolock(hashInFile, 0ll, SEEK_SET);
		rewind(hashInFile);
		while ((nread = _fread_nolock_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size())
			xhashtable.Pass0_BuildHistogram(elem);
		//xhashtable.loadHistogram();
	}
	iteration_clk = clock() - iteration_clk;
	summary_clk += iteration_clk;

	std::wcerr << "Time histogram build : " << (iteration_clk / CLOCKS_PER_SEC) << 's' << std::endl;

	ConfigReader	ConfigEx;

	for (xhashtable.beginLimiters(); xhashtable.endLimiters(ConfigEx.getValue("startLimitGB", 24ull) << 30u); xhashtable.incLimiters())
	{
		decltype(collisions)		lcollisions		= decltype(collisions)();
		decltype(elements_count)	lelements_count = decltype(elements_count)();

		std::wcerr << ": proccessed at index : "; //<< NINDEX;
		xhashtable.print_status_range(std::wcerr);

		//_fseeki64_nolock(hashInFile, 0ll, SEEK_SET);
		rewind(hashInFile);

		iteration_clk = clock();

		for (unsigned int i = std::thread::hardware_concurrency(); i > 0; --i)
			Threads.push_back(std::move(std::unique_ptr<std::thread>(new std::thread(
				[&]()
		{
			alignas(32u)	mytype elem;
			size_t nread;
			decltype(collisions)		tcollisions		= decltype(collisions)();
			decltype(elements_count)	telements_count = decltype(elements_count)();

			while ((nread = fread_s((char*)&elem[0], elem.size(), 1u, elem.size(), hashInFile)) == elem.size())
			{
				auto resins = xhashtable.insert(elem);
				if (resins.first)
				{
					if (resins.second);
					//ms.SaturateSpin(elem.data(), elem.size());
					else
						++tcollisions;
					++telements_count;
				}
			}
			_InterlockedAdd64(reinterpret_cast<volatile long long*>(&lcollisions),		static_cast<long long>(tcollisions));
			_InterlockedAdd64(reinterpret_cast<volatile long long*>(&lelements_count),	static_cast<long long>(telements_count));
		}))));

		for (auto& it : Threads)	it->join();
		Threads.clear();

		iteration_clk = clock() - iteration_clk;
		summary_clk += iteration_clk;

		collisions		+=	lcollisions;
		elements_count	+=	lelements_count;

		std::wcerr << "\tclock(" << iteration_clk << "ms) cols(" << lcollisions << ") count(" << lelements_count << ')' << std::endl;

		//save	----------------------------------------------------------------
		xhashtable.for_range([hashOutFile](const mytype& in) { _fwrite_nolock((char*)&in.at(0), 1u, in.size(), hashOutFile); });
		//----------------------------------------------------------------------
	}

	out << "\n\tcollisions     : " << collisions;
	out << "\n\telements_count : " << elements_count;
	out << "\n\ttime work      : " << (summary_clk / CLOCKS_PER_SEC) << 's';

	//out << "\nMaterialSpector: dump\n";
	//ms.dump(out);
	out << '\n';
	return true;
}