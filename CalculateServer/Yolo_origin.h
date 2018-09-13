#pragma once

#include"Yolo_Result.h"

#include"opencv2/opencv.hpp"
class Yolo_origin{
private:
	static Yolo_Result postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, cv::dnn::Net& net);
	static std::vector<cv::String>	getOutputsNames(const cv::dnn::Net& net);
public:
	static bool ready(int argc,const char* cmd[]);
	static Yolo_Result YOLO(cv::Mat& input);
	static std::vector<std::string> GetClasses();
};