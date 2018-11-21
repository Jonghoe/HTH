#pragma once
#include"opencv2/opencv.hpp"

struct Command{
	float t, r, p, k;
	Command(float _t,float _r,float _p, float _k):t(_t),r(_r),p(_p),k(_k){}
};
class CommandMaker {
public:
	virtual Command makeCommand(const cv::Mat&) = 0;
	virtual int getCommandLen() = 0;
};