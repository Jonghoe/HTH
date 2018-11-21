#pragma once
#include"Communicator.h"
#include"ImagePostprocess.h"
#include"RecvImage.h"
#include"CommandMaker.h"
void CommandCommunicatorMain(SOCKET);
class CommandCommunicator :public Communicator{
private:
	RecvImage recver;
	CommandMaker* commandMaker;
	ImageProcess* imgProcessor;
	static CommandCommunicator* m_pInstance;
	CommandCommunicator(ImageProcess* imgPro,CommandMaker* cmdMaker) :Communicator(CommandCommunicatorMain) {
		if (imgPro != nullptr) {
			imgProcessor = imgPro;
		}
		if (cmdMaker != nullptr) {
			commandMaker = cmdMaker;
		}
	}
public:
	static CommandCommunicator* GetInstance(ImageProcess* imgPro = nullptr,CommandMaker* cmdMaker = nullptr)
	{
		if (m_pInstance == nullptr) {
			m_pInstance = new CommandCommunicator(imgPro,cmdMaker);
		}
		
		return m_pInstance;
	}
	virtual bool isReady();
public:
	friend void CommandCommunicatorMain(SOCKET client);
	
};