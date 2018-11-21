#pragma once
#pragma warning (disable:4996)

// user defined
#include"ImagePostprocess.h"

// opencv header
#include"opencv2/opencv.hpp"


// ffmpeg header
extern "C" {
	#include <libavcodec/avcodec.h>
	#include<libswscale/swscale.h>
	#include<libavformat/avio.h>
	#include<libavformat/avformat.h>
	#include<libavcodec/avcodec.h>		
}

class VideoDecoder 
{
private:
	VideoDecoder(){}
public:
	static VideoDecoder* GetInstance();
	bool ready();
	static void avframeToMat(const AVFrame * frame, cv::Mat& image);
	int parse(uint8_t* pBuff, int videosize, uint64_t pts);
	int parse(uint8_t* buffer, int size);

private:
	static VideoDecoder* m_pInstance;
	AVCodecParserContext* m_pCodecPaser;
	AVCodec* m_pAVCodec;
	AVCodecContext *m_pCodecCtx;
	AVFrame* m_pFrame;

};