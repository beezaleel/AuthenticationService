#pragma once

#include "Buffer.h"
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

struct ClientInfo {
	Buffer buffer;
	SOCKET socket;
	unsigned int packetLength;
	bool connected;
};

struct ServerInfo
{
	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	SOCKET listenSocket = INVALID_SOCKET;
	fd_set activeSockets;
	fd_set socketsReadyForReading;
	std::vector<ClientInfo> clients;
};

enum MessageType {
	Register = 4,
	Login = 5
};

class AuthServer
{
public:
	AuthServer();
	~AuthServer();
	int Bind();
	void Initialize();
	int Listen();
	void Process();
	int Receive(ClientInfo& client, const int bufLen, char* buf);
	int Send(ClientInfo& client, char buf[], int receiveResult);
	void ShutDown();

private:
	ServerInfo m_serverInfo;
	WSADATA m_wsaData;
	void AddConnectedSockets();
	void Accept();
	void GetServerAddrInfo();
	void CreateSocket();
	void Startup();

};