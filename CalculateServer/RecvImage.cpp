#include "RecvImage.h"


cv::Mat RecvImage::recvIMG()
{
	while (pipe->isEmpty()) {
		cv::waitKey(1);
	}
	return pipe->pop();
}
