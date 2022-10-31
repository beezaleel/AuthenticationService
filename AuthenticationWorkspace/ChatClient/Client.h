#pragma once

#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

struct ClientInfo
{
	struct addrinfo* info = nullptr;
	struct addrinfo* ptr = nullptr;
	struct addrinfo hints;
	SOCKET connectSocket = INVALID_SOCKET;
};

class Client
{
public:
	Client();
	~Client();
	int Connect();
	void Initialize();
	int ManageSocket();
	int Receive(char buf[], int bufLen);
	int Send(const char buf[], int bufLen);
	int ShutDown();

private:
	ClientInfo m_clientInfo;
	WSADATA m_wsaData;
	void CreateSocket();
	void GetServerAddrInfo();
	void Startup();
};
