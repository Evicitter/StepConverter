#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace appTR
{

class ChemicalBurner
{
private:
	static const unsigned int		BPAD = 1024u;

	alignas(16u) __m128i			m[256u];
	alignas(16u) unsigned char		buffer[BPAD];

	unsigned long long				length;
	unsigned int					counter;
	unsigned int					crcc;

public:
	ChemicalBurner();

	void exec1(const unsigned char* input);
	void exec2(const unsigned char* input);
	void exec3(const unsigned char* input);
	void exec4(const unsigned char* input);
	void exec5(const unsigned char* input);
	void exec6(const unsigned char* input);
	void exec7(const unsigned char* input);
	//void exec8(const unsigned char* input);

	void Saturate(const unsigned char* buf, size_t len);
	void Finish();

	bool load(const std::wstring& fname);
	bool save(const std::wstring& fname) const;
	void merge(const ChemicalBurner& other);
	void dump(std::wostream& out) const;

private:

	void Execute(const unsigned char* buf);
};

extern std::unique_ptr<ChemicalBurner>	GChemicalBurner;
}