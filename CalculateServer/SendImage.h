#pragma once
#include"ImagePostprocess.h"
#include"ImagePipe.h"

class SendImage {
	ImagePipe* pipe;
	ImageProcess* processor;
public:
	SendImage(): processor(nullptr){
		pipe = ImagePipe::GetInstance();
	}
	void setImageProcessor(ImageProcess* imgPro)
	{
		processor = imgPro;
	}
	void send(cv::Mat& img);
};