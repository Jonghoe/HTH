#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

class Connector
{
private:
	const int port = 9999;
	Connector();
	static Connector* instance;
public:
	static Connector* GetInstance();	
	bool readyToConnect();
	SOCKET acceptClient();
	~Connector();
};

