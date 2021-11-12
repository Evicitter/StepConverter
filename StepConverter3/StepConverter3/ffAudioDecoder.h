//Last update time : 17.10.2013 01:09:41
#pragma once
#include <omp.h>
#include "ffmpegInclude.h"

#define MaxAudioStreams 16

class AudioDecoder
{
public:
	AudioDecoder();
	virtual ~AudioDecoder();

	bool hasOpenStream;

	// open audio stream.
	virtual int OpenAudio(const AVFormatContext* FCtx);
	// close audio stream.
	virtual int CloseAudio();

	// Decode audio from packet.
	//int DecodeAudio(int nStreamIndex, const AVPacket *avpkt, uint8_t* pOutBuffer, size_t nOutBufferSize);
	int DecodeAudio(int nIndexAudio, const AVPacket *avpkt, uint8_t* pOutBuffer, size_t nOutBufferSize);
	
	int ValidPacket(AVPacket* packet) const;
	int Pump(AVPacket* packet);

	//Delegate
	void (*notifyaudioframe)(const unsigned char* bytes, const int size, const int typeframe);

private:
	omp_lock_t	omplocks[MaxAudioStreams];
	uint8_t * pFrameAudio[MaxAudioStreams];
	AVCodecContext* pAudioCodecCtx[MaxAudioStreams];
	AVCodec* pAudioCodec[MaxAudioStreams];
	double audioBaseTime[MaxAudioStreams];
	int audioStreamIndex[MaxAudioStreams];
	const AVFormatContext* pFormatCtx;
};