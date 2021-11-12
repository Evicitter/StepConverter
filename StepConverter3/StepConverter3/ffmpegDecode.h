//Last update time : 22.09.2013 17:13:02
#pragma once
#include <string>
#include "ffmpegInclude.h"
#include "ffAudioDecoder.h"
#include "ffVideoDecoder.h"

class FFmpegDecoder
{
public:
	FFmpegDecoder();
	virtual ~FFmpegDecoder();

	// Open file
	virtual bool OpenFile(std::wstring& inputFile);
	// Close file and free resourses.
	virtual bool CloseFile();

	// Return next frame FFmpeg.
	virtual int GetNextFrame();
	//Begin processing
	virtual int StartWork();

private:
	char filename[1024]; //bad var decl
	AVFormatContext* pFormatCtx;
	AudioDecoder* AudioDecoderInstance;
	VideoDecoder* VideoDecoderInstance;
	bool isOpen; // File is open or not.
};