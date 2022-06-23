#pragma once

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <mutex>
#include <nmmintrin.h>

#include <core/hashes/HashClasses.hpp>
#include "FilesManager.hpp"

namespace appTR {

template<class HASHCLASS>
class HashAccumulator
{
private:
	typedef std::array<unsigned char, HASHCLASS::_HASHSIZE>		mytype;

	std::mutex				mylock;
	FileUnit				blocks;
	std::wstring			myname;
public:

	HashAccumulator() : blocks(L"")
	{
		myname = std::wstring(L"GHI_") + VX::hash::getHashClassNameW<HASHCLASS>(HASHCLASS());
	}

	void update(const unsigned char* buf, const size_t nsize)
	{
		HASHCLASS						objhash;
		objhash.start();
		objhash.update(buf, nsize);
		objhash.finish();
		
		mylock.lock();
		blocks.Write(objhash.getHashPtr(), objhash.getHashSize());
		mylock.unlock();
	}

	bool load(const std::wstring& outdirectory)
	{
		blocks.Rename(outdirectory + myname + L"_tmp_.story", true);
		if (blocks.OpenWrite())
		{
			blocks.setDeleteOnClose(true);
			//blocks.Prealloc(4u);
			return true;
		}
		throw std::runtime_error("Temporary file isn\'t create.");
		return false;
	}

	bool save(const std::wstring& outdirectory, const time_t& timeinput)
	{
		std::wostringstream	ss;
		tm					stime;
		unsigned int		counter = 0u;

		blocks.setDeleteOnClose(false);
		blocks.Flush();
		blocks.Close();

		do
		{
			ss.str(L"");
			ss.fill(L'0');
			ss << outdirectory; // << L"HASHES\\";

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
		while ( !blocks.Rename(ss.str()) && (counter < 100u));

		return true;
	}

	void merge(const std::wstring& fromname, const std::wstring& toappend)
	{
		FileUnit	fileIn(fromname);
		FileUnit	fileOut(toappend + myname + L".story");

		if (!fileIn.OpenRead()) //_wfopen_s(&fileIn, fromname.c_str(), L"rbS")
		{
			std::wcerr << "File \'" << fromname << " access denied.\n";
			return;
		}

		if (!fileOut.OpenAppend()) //_wfopen_s(&fileOut, (toappend + myname + L".story").c_str(), L"ab"))
		{
			std::wcerr << "File \'" << toappend << myname << ".story\' don\'t save.\n";
			return;
		}

		unsigned char	buffer[65536u];
		size_t			nread;

		//file copy.
		while ( nread = fileIn.Read(buffer, sizeof(buffer)) )
						fileOut.Write(buffer, nread);
		
		fileOut.Flush();
	}

	void dump(const std::wstring& fname, std::wostream& out) const
	{
		//std::array<unsigned long long, HASHCLASS::_HASHSIZE>	indexdestrib{0ull};
		unsigned long long crc = 0x00000000ffffffffull;
		unsigned long long countelems = 0ull;

		FileUnit infile(fname + myname + L".story");

		if(infile.OpenRead())
		{
			std::array< mytype, 4096u >	elems;
			size_t	nread;
			while (nread = infile.Read(elems.data(), elems.size() * elems[0].size()))
			{
				nread /= elems[0].size();
				for (unsigned int i = 0u; i < nread; ++i)
				{
					++countelems;
					//++indexdestrib[elem.at(0) & (indexdestrib.size() - 1u)];
					for (unsigned int j = 0u; j < elems[0].size(); j += 8u)
						crc = _mm_crc32_u64(crc, *reinterpret_cast<const unsigned long long*>(&elems[i].at(j)));
				}
			}

			infile.Close();

			out << "\tCRC(";
			out.width(2u); out << elems[0].size() << ',';
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

}