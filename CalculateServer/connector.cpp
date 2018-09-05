#include "connector.h"



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
	struct sockaddr_in server;
	if (WSAStartup(MAKEWORD(2, 2), &gWSA )!= 0) {
		return false;
	}

	if ((gServerSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		return false;
	}
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	int bindRetV = bind(gServerSocket, (struct sockaddr*)&server, sizeof(server));
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
	gClientSocket = accept(gClientSocket, NULL, NULL);
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
