#pragma once

#include <SDKDDKVer.h>

#include <iostream>
#include <string>
#include <memory>

namespace appTR {

extern std::wstring	OutputDirectory;

//extern void ShowVersion();
class TimeInOut
{
public:
	TimeInOut();
	~TimeInOut();

	long long	getTime() const { return	starttime; }

	void	ShowNow(std::wostream& out = std::wcerr)	const;
protected:
	void	printTime(const tm& stime, std::wostream& out)	const;
	long long starttime;
};

extern void LoadHashAccums(const std::wstring& todir);
extern void SaveHashAccums(const std::wstring& todir);
extern void LoadResults(const std::wstring& todir);
extern void SaveResults(const std::wstring& todir);

extern int ShowDump(const bool bShowHelp = false);
extern int MergeResults(const bool bShowHelp = false);
extern int ExportLayers(const bool bShowHelp = false);
extern int Split(const std::wstring& openfile, const std::wstring& outdir, const size_t	countfiles, const size_t element_size, const bool bShowHelp = false);
extern int doRandom(TimeInOut& tio, const bool bShowHelp = false);

extern int Check(const bool bShowHelp = false);
extern int Unique(const bool bShowHelp = false);

}