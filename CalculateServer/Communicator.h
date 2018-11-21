#pragma once

#include<string>
#include<thread>
#include "connector.h"

typedef void (*PlayInterface)(SOCKET);
class Communicator {
private:
	SOCKET client;
	std::thread m_pThread;
	std::string m_pIp;
	int m_pPort;
	bool makeServer();
	bool waitAccept();
	PlayInterface m_pFunc;

protected:
	Communicator(PlayInterface play);

public:
	void join();
	void play();
	void setIP(const std::string&);
	void setPort(int port);
	virtual bool isReady()=0;
};

