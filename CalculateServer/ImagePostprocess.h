#pragma once
#include"opencv2/opencv.hpp"
class ImageProcess 
{
public:
	virtual void process(cv::Mat&) = 0;
};