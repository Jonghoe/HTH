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
	static void avframeToMat(const AVFrame * frame, cv::Mat& image);

public:
	static bool ready(const char* ip);
	static void Decode(ImageProcess*);

private:
	static uint8_t* buffer;
	static int len;
};