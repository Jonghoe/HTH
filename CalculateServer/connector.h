#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

class Connector
{
private:
	static Connector* instance;


private:
	Connector();
public:
	static Connector* GetInstance();	
	bool readyToConnect(const char ip[], int port);
	SOCKET acceptClient();
	~Connector();
};

