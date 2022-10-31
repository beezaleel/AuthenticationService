#include "Client.h"
#include "Buffer.h"

/// <summary>
/// Constructor
/// </summary>
Client::Client()
{
}

/// <summary>
/// Destructor
/// </summary>
Client::~Client()
{
}

/// <summary>
/// Connect to server
/// </summary>
/// <returns></returns>
int Client::Connect() {
	int state = -1;
	printf("Connect to the server . . . \n");
	state = 
		connect(
			m_clientInfo.connectSocket, 
			m_clientInfo.info->ai_addr, 
			(int)m_clientInfo.info->ai_addrlen
		);
	if (state == SOCKET_ERROR) {
		printf("Failed to connect to server. Error - %d\n", WSAGetLastError());
		closesocket(m_clientInfo.connectSocket);
		freeaddrinfo(m_clientInfo.info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Connected to server Successful!\n");
	}

	return state;
}

/// <summary>
/// Initialization of components
/// </summary>
void Client::Initialize() {
	// Create and startup WSADATA
	printf("Initializing client components! \n");

	// Create WSAStartup
	Startup();

	// Create getaddrinfo
	GetServerAddrInfo();

	// Create Socket
	CreateSocket();
}

/// <summary>
/// Manage socket
/// </summary>
/// <returns></returns>
int Client::ManageSocket() {
	DWORD NonBlock = 1;
	int state = -1;
	state = ioctlsocket(m_clientInfo.connectSocket, FIONBIO, &NonBlock);
	if (state == SOCKET_ERROR) {
		printf("Socket manager (ioctlsocket) failed. Error - %d\n", WSAGetLastError());
		closesocket(m_clientInfo.connectSocket);
		freeaddrinfo(m_clientInfo.info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Created ioctlsocket!\n");
	}

	return state;
}

/// <summary>
/// The receiver
/// </summary>
/// <param name="buf">The buffer</param>
/// <param name="bufLen">Length</param>
/// <returns></returns>
int Client::Receive(char buf[], int bufLen) {
	int state = -1;
	Buffer buffer = Buffer(bufLen);
	state = recv(m_clientInfo.connectSocket, (char*)&(buffer.Data[0]), bufLen, 0);
	if (state == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			//printf("WouldBlock!\n");
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(m_clientInfo.connectSocket);
			WSACleanup();
			return 1;
		}
	}
	else {
		char* message = buffer.ReadString(0);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		printf("%s", message);
		printf("\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
	}

	return state;
}

/// <summary>
/// Send message
/// </summary>
/// <param name="buf"></param>
/// <param name="bufLen"></param>
/// <returns></returns>
int Client::Send(const char buf[], int bufLen) {
	int state = -1;
	state = send(m_clientInfo.connectSocket, buf, bufLen, 0);
	if (state == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(m_clientInfo.connectSocket);
		WSACleanup();
		return 1;
	}
	else {
		//printf("Success!\n");
		//printf("Sent %d bytes to the server!\n", state);
	}

	return state;
}

/// <summary>
/// Shutdown client
/// </summary>
/// <returns></returns>
int Client::ShutDown() {
	int state = -1;
	printf("Shuting down client . . .\n");
	state = shutdown(m_clientInfo.connectSocket, SD_SEND);
	if (state == SOCKET_ERROR) {
		printf("Shutdown failed due to Error - %d\n", WSAGetLastError());
		closesocket(m_clientInfo.connectSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Shutdown was Successful!\n");
	}

	printf("Closing . . . \n");
	closesocket(m_clientInfo.connectSocket);
	WSACleanup();

	return state;
}

/// <summary>
/// Create socket
/// </summary>
void Client::CreateSocket() {
	m_clientInfo.connectSocket =
		socket(
			m_clientInfo.info->ai_family,
			m_clientInfo.info->ai_socktype,
			m_clientInfo.info->ai_protocol
		);
	if (m_clientInfo.connectSocket == INVALID_SOCKET) {
		printf("Failed to Create Socket. Error - %d\n", WSAGetLastError());
		freeaddrinfo(m_clientInfo.info);
		WSACleanup();
		exit(1);
	}
	else {
		printf("Socket was created successful!\n");
	}
}

/// <summary>
/// The address info
/// </summary>
void Client::GetServerAddrInfo() {
	int state = -1;

	ZeroMemory(&m_clientInfo.hints, sizeof(m_clientInfo.hints));
	m_clientInfo.hints.ai_family = AF_INET;
	m_clientInfo.hints.ai_socktype = SOCK_STREAM;
	m_clientInfo.hints.ai_protocol = IPPROTO_TCP;

	state = getaddrinfo("127.0.0.1", "8888", &m_clientInfo.hints, &m_clientInfo.info);
	if (state != 0) {
		printf("Failed to Create getaddrinfo. Error - %d\n", state);
		WSACleanup();
		exit(1);
	}
	else {
		printf("getaddrinfo was successful!\n");
	}
}

/// <summary>
/// WSAtartup initialization
/// </summary>
void Client::Startup() {
	int state = -1;
	WORD wVersionRequested = MAKEWORD(2, 2);

	state = WSAStartup(wVersionRequested, &m_wsaData);
	if (state != 0) {
		printf("Failed to startup WSAStartup. Error - %d\n", state);
		exit(1);
	}
	else {
		printf("WSAStartup was successful! \n");
	}
}
