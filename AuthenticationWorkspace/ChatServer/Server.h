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

struct User {
	std::string name;
	SOCKET socket;
};

struct Room {
	std::string roomName;
	std::vector<User> members;
};

struct ServerInfo
{
	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	SOCKET listenSocket = INVALID_SOCKET;
	fd_set activeSockets;
	fd_set socketsReadyForReading;
	std::vector<ClientInfo> clients;
	std::vector<Room> rooms;
};

enum MessageType {
	Join = 1,
	Leave = 2,
	Send = 3
};

class Server
{
public:
	Server();
	~Server();
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
	void PrintStatusInfo();
	void CreateSocket();
	void Startup();
};
