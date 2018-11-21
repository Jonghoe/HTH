#include "SendImage.h"

void SendImage::send(cv::Mat & img)
{
	if (processor != nullptr) {
		processor->process(img);
	}
	pipe->push(img);
}
