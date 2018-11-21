#include "Communicator.h"
bool Communicator::makeServer()
{
	Connector* connect = Connector::GetInstance();
	if (!connect->readyToConnect(m_pIp.c_str(), m_pPort)) {
		return false;
	}
	return true;
}

bool Communicator::waitAccept()
{
	Connector* connect = Connector::GetInstance();
	client = connect->acceptClient();
	return client != NULL;
}



void Communicator::join()
{
	m_pThread.join();
}

void Communicator::play()
{
	bool ok = isReady();
	if (!ok){
		exit(-1);
	}
	ok = makeServer();
	if (!ok) {
		exit(-1);
	}
	ok = waitAccept();
	if (!ok) {
		exit(-1);
	}
	m_pThread = std::thread(m_pFunc,client);
}

void Communicator::setIP(const std::string& ip)
{
	m_pIp = ip;
}

void Communicator::setPort(int port)
{
	m_pPort = port;
}

Communicator::Communicator(PlayInterface play):m_pFunc(play),m_pIp(),m_pPort(0)
{
}
