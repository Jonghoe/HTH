#pragma once
#include"Communicator.h"
#include"ImageCommunicator.h"
#include"CommandCommunicator.h"
#include"opencv2/opencv.hpp"


class DroneController
{
	Communicator* imgCommunicator;
	Communicator* cmdCommunicator;
public:
	DroneController();
	void setIMGReceverFunc(IMGRecever);
};