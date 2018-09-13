#pragma once
#include "ImagePostprocess.h"
class ViewProcess : public ImageProcess{
public:
	virtual void process(cv::Mat&);
	ViewProcess(){}
};