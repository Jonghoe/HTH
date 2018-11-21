#include "TempCommandMaker.h"

Command TempCommandMaker::makeCommand(const cv::Mat & img)
{
	return Command(1.1,2.2,3.3,4.4);
}

int TempCommandMaker::getCommandLen()
{
	return 16;
}
