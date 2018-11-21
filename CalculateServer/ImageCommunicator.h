#pragma once
#include "Communicator.h"
#include "ImagePostprocess.h"
#include "ImagePipe.h"
#include "SendImage.h"
typedef cv::Mat(*IMGRecever)(char* buffer, int len);

void IMGCommunicatorMain(SOCKET client);

class ImageCommunicator :public Communicator {
private:
	static ImageCommunicator* m_pInstance;
	IMGRecever imgRecever;
private:
	ImageCommunicator(ImageProcess* imgPro) :Communicator(IMGCommunicatorMain),sender(){
		if(imgPro != nullptr){
			sender.setImageProcessor(imgPro);
		}
	}
public:
	static ImageCommunicator* GetInstance(ImageProcess* imgPro = nullptr) {
		if (m_pInstance == nullptr)
			m_pInstance = new ImageCommunicator(imgPro);
		return m_pInstance;
	}
	friend void IMGCommunicatorMain(SOCKET client);
	virtual bool isReady();
	SendImage sender;

};

