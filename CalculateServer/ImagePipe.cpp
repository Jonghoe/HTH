#include "ImagePipe.h"
ImagePipe* ImagePipe::m_pInstance = nullptr;

void ImagePipe::send_complete()
{
}

bool ImagePipe::isEmpty()
{
	return m_pIMGPipe.empty();
}

void ImagePipe::push(cv::Mat & img)
{
	thread_mutex.lock();
	if (m_pIMGPipe.size() > 1) {
		m_pIMGPipe.pop();
	}
	m_pIMGPipe.push(img);
	thread_mutex.unlock();
	send_complete();
}

cv::Mat ImagePipe::pop()
{
	thread_mutex.lock();
	cv::Mat img;
	if (m_pIMGPipe.empty())
		img = cv::Mat();
	else {
		img = m_pIMGPipe.front();
		m_pIMGPipe.pop();
	}
	thread_mutex.unlock();
	return img;
}
