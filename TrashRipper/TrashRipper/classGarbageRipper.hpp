#pragma once

#include <iostream>
//#include <fstream>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <nmmintrin.h>

#include <stdio.h>

#include "classHashAccumulator.hpp"
#include "classChemicalBurner.hpp"

class GarbageRipper
{
public:
	GarbageRipper() : filehandle(nullptr), filesizeMB(0u)
	{}

	bool	RipFile(const std::wstring& fname);

private:
	std::mutex									lockfile;
	std::vector< std::unique_ptr<std::thread> >	threads;
	FILE*										filehandle;
	long long									filesizeMB;
	clock_t										lastclock;
};

class DictionaryRipper
{
public:
	DictionaryRipper() : filehandle(nullptr)
	{}

	bool	RipFile(const std::wstring& fname);

private:
	FILE*	filehandle;

	struct AccumX
	{
	private:
		std::vector<unsigned char>	accum;
	public:
		AccumX();
		void Push(const unsigned char in);
		void Flush();
	};
};