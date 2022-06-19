#pragma once

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//#include <mutex>
#include <nmmintrin.h>

#include <core/hashes/HashClasses.hpp>

template<class HASHCLASS>
class HashAccumulator
{
private:
	typedef std::array<unsigned char, HASHCLASS::_HASHSIZE>		mytype;

	//std::mutex				mylock;
	//std::vector<mytype>		blocks;
	FILE*					blocks;

	std::wstring			myname;
public:

	HashAccumulator() : blocks(nullptr)
	{
		//blocks.reserve(65536u * 2u); //131 072 frames ~(30fps) = 1 h 12 min
		myname = std::wstring(L"GHI_") + VX::hash::getHashClassNameW<HASHCLASS>(HASHCLASS());
	}

	void update(const unsigned char* buf, const size_t nsize)
	{
		HASHCLASS						objhash;
		objhash.start();
		objhash.update(buf, nsize);
		objhash.finish();
#if 0
		//version 1
		{
			std::lock_guard<std::mutex> ml(mylock);
			size_t curidx = blocks.size();
			blocks.resize(curidx + 1u);
			objhash.print(blocks[curidx]);
		}
#elif 0
		//version 2
		mytype							reshash;
		objhash.print(reshash);
		{
			std::lock_guard<std::mutex> ml(mylock);
			blocks.emplace_back(reshash);
		}
#else
		//version 3
		fwrite(objhash.getHashPtr(), 1u, objhash.getHashSize(), blocks);
#endif
	}

	bool load(const std::wstring& fname)
	{
		//blocks.clear();

		if (_wfopen_s(&blocks, (fname + myname + L"_tmp_.story").c_str(), L"wb") )
			throw std::runtime_error("Temporary file isn\'t create.");
		else
			setvbuf(blocks, nullptr, _IOFBF, 8u << 20u);

		//std::ifstream ifsstory(fname + myname + L".story", std::ofstream::binary | std::ofstream::in);
		//if (!(ifsstory && ifsstory.is_open()))
		//	std::wcerr << "File \'" << fname << myname << ".story\' don\'t opened.\n";
		//bret = ifsstory.good();
		//ifsstory.close();

		return true;
	}

	bool save(const std::wstring& fname, const time_t& timeinput) const
	{
		std::wostringstream	ss;
		tm					stime;
		unsigned int		counter = 0u;

		fflush(blocks);
		fclose(blocks);

		do
		{
			ss.str(L"");
			ss.fill(L'0');
			ss << fname; // << L"HASHES\\";

			if (localtime_s(&stime, &timeinput) == 0)
			{
				ss << (stime.tm_year + 1900) << '-';
				ss.width(2u); ss << (stime.tm_mon + 1) << '-';
				ss.width(2u); ss << stime.tm_mday << "__";
				ss.width(2u); ss << stime.tm_hour << '-';
				ss.width(2u); ss << stime.tm_min << '-';
				ss.width(2u); ss << stime.tm_sec;
				if(counter)
				{
					ss << '-';
					ss.width(2u);	ss	<<	counter;
				}
			}
			else
			{
				unsigned long long	hnumber = rand() | (rand() << 15u);
				hnumber |= (static_cast<unsigned long long>(rand()) << 30u) | (static_cast<unsigned long long>(rand()) << 45u) | (static_cast<unsigned long long>(rand()) << 60u);
				ss.width(16u); ss << std::hex << hnumber;
			}
			ss << '.' << myname;
			++counter;
		}
		while( _wrename((fname + myname + L"_tmp_.story").c_str(), ss.str().c_str()) && (counter < 100u));

		return true;
	}

	void merge(const std::wstring& fromname, const std::wstring& toappend)
	{
		FILE*	fileIn;
		FILE*	fileOut;

		if (_wfopen_s(&fileIn, fromname.c_str(), L"rbS"))
		{
			std::wcerr << "File \'" << fromname << " access denied.\n";
			return;
		}

		if (_wfopen_s(&fileOut, (toappend + myname + L".story").c_str(), L"ab"))
		{
			fclose(fileIn);
			std::wcerr << "File \'" << toappend << myname << ".story\' don\'t save.\n";
			return;
		}

		unsigned char	buffer[65536u];
		size_t			nread;

		//file copy.
		while ( nread = _fread_nolock_s(buffer, sizeof(buffer), 1, sizeof(buffer), fileIn) )
						_fwrite_nolock (buffer,                 1,          nread, fileOut);
		
		fflush(fileOut);
		fclose(fileOut);
		fclose(fileIn);
	}

	void dump(const std::wstring& fname, std::wostream& out) const
	{
		//std::array<unsigned long long, 16u>	indexdestrib{0ull};
		unsigned long long crc = 0x00000000ffffffffull;
		unsigned long long countelems = 0ull;

		std::ifstream ifsstory(fname + myname + L".story", std::ifstream::binary | std::ifstream::in);
		if (ifsstory && ifsstory.is_open())
		{
			mytype elem;
			ifsstory.read((char*)&elem[0], elem.size());
			while (ifsstory.gcount() == elem.size())
			{
				++countelems;
				//++indexdestrib[elem.at(0) & (indexdestrib.size() - 1u)];
				for (unsigned int x = 0; x < elem.size(); x += 8u)
					crc = _mm_crc32_u64(crc, *reinterpret_cast<unsigned long long*>(&elem.at(x)));
				ifsstory.read((char*)&elem[0], elem.size());
			}
			ifsstory.close();

			out << "\tCRC(";
			out.width(2u); out << elem.size() << ',';
			out.width(8u); out << countelems << ") = " << std::hex;
			out.width(8u); out << crc << std::dec << '\t';
			//out << "index_destrib( ";
			//for(size_t i=0u; i<indexdestrib.size(); ++i)	out << indexdestrib[i] << ',';
			//out << ' ' << ')';
		}
	}
};

template<class HASHCLASS>
class HashAccumulatorDummy
{
private:
	typedef std::array<unsigned char, HASHCLASS::_HASHSIZE>		mytype;
public:

	HashAccumulatorDummy()	{	}

	void update(const unsigned char* buf, const size_t nsize)			{					}
	bool load(const std::wstring& fname)								{	return true;	}
	bool save(const std::wstring& fname, const time_t& timeinput) const	{	return true;	}
	void merge(const std::wstring& fromname, const std::wstring& toappend)	{				}
	void dump(const std::wstring& fname, std::wostream& out) const			{				}
};

//128 bit hashes
extern HashAccumulatorDummy<VX::hash::RHash>				GHI_RHash;

//256 bit hashes
extern HashAccumulator<VX::hash::DRIB>		GHI_DRIB;
extern HashAccumulator<VX::hash::AESDM>		GHI_AESDM;
extern HashAccumulator<VX::hash::AESMMO2>	GHI_AESMMO2;
extern HashAccumulator<VX::hash::AESHIROSE>	GHI_AESHIROSE;