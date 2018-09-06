#include "connector.h"

static WSADATA gWSA;
static SOCKET gClientSocket, gServerSocket;
static int gClientLen;
static struct sockaddr_in gServerAddr,gClientAddr;

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

bool Connector::readyToConnect()
{
	if (WSAStartup(MAKEWORD(2, 2), &gWSA )!= 0) {
		return false;
	}

	if ((gServerSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		return false;
	}
	gServerAddr.sin_addr.s_addr = inet_addr("192.168.0.2");
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
	gClientLen = sizeof(struct sockaddr_in);
	gClientSocket = accept(gServerSocket, (struct sockaddr *)&gClientAddr, &gClientLen);
	if (gClientSocket == INVALID_SOCKET)
		return NULL;
	else
		return gClientSocket;
}

Connector::~Connector()
{
	if (instance != NULL)
		delete instance;
}
