#pragma once
#include "CommandMaker.h";

class TempCommandMaker:public CommandMaker
{
public:
	TempCommandMaker(){}

	virtual Command makeCommand(const cv::Mat&) ;
	virtual int getCommandLen() ;
};