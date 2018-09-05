#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

WSADATA gWSA;
static SOCKET gClientSocket, gServerSocket;
class Connector
{
private:
	const int port = 8888;
	Connector();
	static Connector* instance;
public:
	static Connector* GetInstance();	
	bool readyToConnect();
	SOCKET acceptClient();
	~Connector();
};

