#include "connector.h"
#include <mutex>
static WSADATA gWSA;
static SOCKET gClientSocket, gServerSocket;
static int gClientLen;
static struct sockaddr_in gServerAddr,gClientAddr;
static std::mutex thread_lock;
Connector* Connector::instance = NULL;

Connector::Connector()
{
}

Connector* Connector::GetInstance()
{
	if (instance == NULL) {
		instance = new Connector();
	}
	return instance;
}

bool Connector::readyToConnect(const char ip[],int port)
{
	thread_lock.lock();
	if (WSAStartup(MAKEWORD(2, 2), &gWSA )!= 0) {
		return false;
	}

	if ((gServerSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		return false;
	}
	gServerAddr.sin_addr.s_addr = inet_addr(ip);
	gServerAddr.sin_family = AF_INET;
	gServerAddr.sin_port = htons(port);
	int bindRetV = bind(gServerSocket, (struct sockaddr*)&gServerAddr, sizeof(gServerAddr));
	if (bindRetV == SOCKET_ERROR) {
		return false;
	}
	int listenResult = listen(gServerSocket, SOMAXCONN);
	if (listenResult == SOCKET_ERROR) {
		closesocket(gServerSocket);
		WSACleanup();
		return false;
	}
	return true;
}

SOCKET Connector::acceptClient()
{
	SOCKET retV;
	gClientLen = sizeof(struct sockaddr_in);
	gClientSocket = accept(gServerSocket, (struct sockaddr *)&gClientAddr, &gClientLen);
	retV = gClientSocket;
	thread_lock.unlock();
	if (retV== INVALID_SOCKET)
		return NULL;
	else
		return retV;
}

Connector::~Connector()
{
	if (instance != NULL)
		delete instance;
}
