//Last update time : 17.10.2013 02:36:40
#pragma once

#include "ffmpegInclude.h"
#include <omp.h>

class VideoDecoder
{
protected:
	bool hasOpenStream;
	
public:
	VideoDecoder();
	virtual ~VideoDecoder();

	// open video stream.
	int OpenVideo(const AVFormatContext* pFCtx);

	// close video stream.
	int CloseVideo();

	// Decode video buffer.
	bool DecodeVideo(const AVPacket *avpkt, AVFrame * pOutFrame);

	//-- call video notify
	int GetVideoFrame(AVFrame *pFrameYuv);
	int GetVideoFrameQueue(AVFrame *pFrameYuv);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	//delegates
	void (*notifyvideoframe)(const unsigned char* bytes, const int size, const int typeframe);

	int Pump(AVPacket* packet);
	
private:	
	int width;
	int height;
	int videoStreamIndex;
	double videoFramePerSecond;
	double videoBaseTime;
	double videoDuration;
	AVCodecContext* pVideoCodecCtx;
	AVCodec* pVideoCodec;
	omp_lock_t	omplockdecoder;
	omp_lock_t	omplocks[32];
	struct SwsContext *pImgConvertCtx[32];
	AVFrame* frames[32];
	uint8_t* ImgBuffers[32];
	const AVFormatContext* pFormatCtx;
};