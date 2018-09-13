#pragma once
#include<vector>
#include"opencv2/opencv.hpp"

struct Yolo_Result {
	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	std::vector<int> indices;
};