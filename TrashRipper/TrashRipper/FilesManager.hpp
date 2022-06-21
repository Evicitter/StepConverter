#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <iostream>

namespace appTR
{

class FileUnit
{
private:
	//std::mutex				FileGlobMutex;
	std::wstring			FileName;
	void*					FileHandle;
	long long				FileSize;
public:
	explicit FileUnit(const std::wstring& _filename) : FileName(_filename), FileHandle(nullptr), FileSize(0ll) {}
	~FileUnit()	{
		Close();
	}

	long long	getFileSize();

	bool	OpenAppend();
	bool	OpenRead();
	bool	Close();
	bool	Rewind();
	bool	Flush();

	bool	Prealloc(const long long PreallocMiB);

	bool	setDeleteOnClose(bool bDelete);
	
	size_t	Read(void*	outBuffer, const unsigned int nRead, std::mutex* _locksync = nullptr);
	size_t	Write(void*	inBuffer, const unsigned int nWrite, std::mutex* _locksync = nullptr);

	bool	Rename(const std::wstring& _newname);
	bool	Delete();

	template<typename E, typename F = [](const E& in, const unsigned int nread)->bool{return false;}>
	bool	while_read(F fun)
	{
		E				buffer;
		unsigned int	nread;
		do
		{
			nread = Read(&buffer, sizeof(buffer));
			if(!nread)	break;
			if(!fun(buffer, nread))	break;
		}while(nread == sizeof(buffer));
		return false;
	}	
};

class FileManagerList
{
public:
	struct _RECFILE {
		std::wstring	fname;
		uint64_t	fsize;
		uint64_t	attr;
		uint64_t	ftime_create;
		uint64_t	ftime_update;
		uint64_t	ftime_open;

		_RECFILE(const wchar_t* _fname, const uint64_t _fsize, const uint64_t _attr,
			const uint64_t _ftime_create, const uint64_t _ftime_update, const uint64_t _ftime_open) : 
			fsize(_fsize), attr(_attr), ftime_create(_ftime_create), ftime_update(_ftime_update), ftime_open(_ftime_open)
		{
			fname = _fname;
		}
	};

	FileManagerList(const std::wstring& _filter = L"*.*")
	{
		cachefiles.reserve(1024u);
		scanFiles(_filter);
	}

	size_t									size()	const	{	return cachefiles.size();	}
	std::vector<_RECFILE>::const_iterator	begin() const	{	return cachefiles.cbegin();	}
	std::vector<_RECFILE>::const_iterator	end()	const	{	return cachefiles.cend();	}

	void			print(std::wostream& out) const;

private:
	bool			scanFiles(const std::wstring& _filter);

	std::vector<_RECFILE>	cachefiles;
};

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

class FilesManager
{
private:

	std::vector<std::wstring>	cachefiles;
	int							fileindex;
	bool						bDeleteFiles;
	bool						bScanFiles;

private:
	bool			scanFiles();

public:
	FilesManager() : fileindex(-1), bDeleteFiles(false), bScanFiles(false) {}
	~FilesManager() {}

	std::wstring	getCurrentFileName();

	bool			NextFileName();
	bool			DeleteCurrentFile();

	//set/get options:
	void			setDeleteFile(bool bcheck)       { bDeleteFiles = bcheck; }
	bool			isDeleteFile(            ) const { return bDeleteFiles; }
	void			setScanFiles( bool bcheck)       { bScanFiles = bcheck; }
	bool			isScanFiles(             ) const { return bScanFiles; }
	//-----------------------------------------------------------------------------

	void			addCacheFile(const std::wstring& fname);

	void			print(std::wostream& out);
};



};