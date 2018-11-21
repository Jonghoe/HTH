#pragma once
#include <queue>
#include <mutex>
#include<opencv2/opencv.hpp>
class ImagePipe
{
private:
	static ImagePipe* m_pInstance;
	std::mutex thread_mutex;
	ImagePipe():thread_mutex(){
	}
	std::queue<cv::Mat> m_pIMGPipe;
public:
	static ImagePipe* GetInstance() {
		if (m_pInstance == nullptr)
			m_pInstance = new ImagePipe();
		return m_pInstance;
	}
	void send_complete();
	bool isEmpty();
	void push(cv::Mat& img);
	cv::Mat pop();
};