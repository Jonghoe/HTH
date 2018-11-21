#pragma once
#include "ImagePostprocess.h"
#include "ImagePipe.h"

class RecvImage{
	ImagePipe* pipe;
public:
	RecvImage() {
		pipe = ImagePipe::GetInstance();
	}
	cv::Mat recvIMG();
};