#include "AuthServer.h"
#include "registration.pb.h"

#define DEFAULT_PORT "9000"

AuthServer::AuthServer()
{
}

AuthServer::~AuthServer()
{
}

/// <summary>
/// Binds the socket to Address
/// </summary>
/// <returns></returns>
int AuthServer::Bind()
{
	int state = -1;
	state =
		bind(
			m_serverInfo.listenSocket,
			m_serverInfo.info->ai_addr,
			(int)m_serverInfo.info->ai_addrlen
		);
	if (state == SOCKET_ERROR) {
		printf("Binding failed due to Error - %d\n", WSAGetLastError());
		freeaddrinfo(m_serverInfo.info);
		closesocket(m_serverInfo.listenSocket);
		WSACleanup();
	}
	else {
		printf("Bind was successful! \n");
	}
	return state;
}

/// <summary>
/// Initial startup of socket. Basic initialization
/// </summary>
void AuthServer::Initialize()
{
	// Create and startup WSADATA
	printf("Initializing server components! \n");

	// Create WSAStartup
	Startup();

	// Create getaddrinfo
	GetServerAddrInfo();

	// Create Socket
	CreateSocket();
}

/// <summary>
/// Listening for active clients
/// </summary>
/// <returns></returns>
int AuthServer::Listen()
{
	int state = -1;
	state = listen(m_serverInfo.listenSocket, SOMAXCONN);
	if (state == SOCKET_ERROR) {
		printf("Failed to listen. Error - %d\n", WSAGetLastError());
		freeaddrinfo(m_serverInfo.info);
		closesocket(m_serverInfo.listenSocket);
		WSACleanup();
	}
	else {
		printf("Authentication Server listening on port %s \n", DEFAULT_PORT);
	}
	return state;
}

/// <summary>
/// Socket processing is done here
/// </summary>
void AuthServer::Process()
{
	struct timeval tVal;
	tVal.tv_sec = 0;
	tVal.tv_usec = 200 * 1000;

	int selectResult;

	while (true) {
		FD_ZERO(&m_serverInfo.socketsReadyForReading);
		FD_SET(m_serverInfo.listenSocket, &m_serverInfo.socketsReadyForReading);

		AddConnectedSockets();

		selectResult = select(0, &m_serverInfo.socketsReadyForReading, NULL, NULL, &tVal);
		if (selectResult == SOCKET_ERROR) {
			printf("select() failed with error: %d\n", WSAGetLastError());
			return;
		}
		printf(".");

		if (FD_ISSET(m_serverInfo.listenSocket, &m_serverInfo.socketsReadyForReading)) {
			printf("\n");
			Accept();
		}

		for (int i = m_serverInfo.clients.size() - 1; i >= 0; i--) {
			ClientInfo& client = m_serverInfo.clients[i];
			if (client.connected == false)
				continue;

			if (FD_ISSET(client.socket, &m_serverInfo.socketsReadyForReading)) {

				const int buflen = 128;
				char buf[buflen];

				int dataStartIndex = client.buffer.Data.size();
				int recvResult = Receive(client, buflen, buf);
			}
		}
	}
}

/// <summary>
/// Handles socket receive
/// </summary>
/// <param name="client">client socket</param>
/// <param name="bufLen">Buffer length</param>
/// <param name="buf">The buffer</param>
/// <returns></returns>
int AuthServer::Receive(ClientInfo& client, const int bufLen, char* buf)
{
	int recvResult = recv(client.socket, (char*)&(client.buffer.Data[0]), bufLen, 0);
	if (recvResult == SOCKET_ERROR) {
		printf("Receive failed. Error - %d\n", WSAGetLastError());
	}

	if (recvResult == 0) {
		printf("Client disconnected!\n");
		client.connected = false;
	}
	else if (recvResult > 0) {
		if (client.buffer.Data.size() >= 4) {
			client.packetLength = client.buffer.ReadUInt32(0);
			short messageId = client.buffer.ReadShort(4);
			unsigned int roomNameSize = client.buffer.ReadUInt32(6);
			char* roomname = client.buffer.ReadString(10);
			account::CreateAccountWeb deserializeUser;
			std::string serializedUser;

			switch (messageId)
			{
			case MessageType::Register:
				bool result;
				serializedUser = client.buffer.ReadString(6);
				result = deserializeUser.ParseFromString(serializedUser);
				std::cout << "email: " << deserializeUser.email() << " password: " << deserializeUser.plaintextpassword() << " id: " << deserializeUser.requestid() << std::endl;

				std::cout << "packagelength: " << client.packetLength << " messageId: " << messageId << " serializedMessage: " << serializedUser << std::endl;
				break;
			default:
				break;
			}


			std::cout << "packagelength: " << client.packetLength << " messageId:" << messageId << " roomNameSize:" << roomNameSize << " roomname:" << roomname << std::endl;

		}

		printf("Bytes received: %d\n", recvResult);
		printf("Message From the client:\n%s\n", buf);
	}
	else {
		printf("Receive failed. Error - %d\n", WSAGetLastError());
	}
	return recvResult;
}

/// <summary>
/// Send message
/// </summary>
/// <param name="client">The client</param>
/// <param name="buf">The buffer</param>
/// <param name="receiveResult">Buffer length</param>
/// <returns></returns>
int AuthServer::Send(ClientInfo& client, char buf[], int receiveResult)
{
	printf("Sending message : %s\n", buf);
	int sendResult = send(client.socket, buf, receiveResult, 0);
	if (sendResult == SOCKET_ERROR) {
		printf("Send failed. Error - %d\n", WSAGetLastError());
		client.connected = false;
	}
	else if (sendResult > 0) {
		printf("Bytes sent: %d\n", sendResult);
	}
	else {
		printf("No response from client\n");
	}
	return sendResult;
}

/// <summary>
/// Shut down socket
/// </summary>
void AuthServer::ShutDown()
{
	printf("Shutting down server . . .\n");
	freeaddrinfo(m_serverInfo.info);
	closesocket(m_serverInfo.listenSocket);
	WSACleanup();
}

/// <summary>
/// Adds conected sockets
/// </summary>
void AuthServer::AddConnectedSockets()
{
	for (int i = 0; i < m_serverInfo.clients.size(); i++) {
		ClientInfo& client = m_serverInfo.clients[i];
		if (client.connected) {
			FD_SET(client.socket, &m_serverInfo.socketsReadyForReading);
		}
	}
}

/// <summary>
/// The socket accept
/// </summary>
void AuthServer::Accept()
{
	SOCKET clientSocket = accept(m_serverInfo.listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("Accept failed with error: %d\n", WSAGetLastError());
	}
	else {
		printf("Accepted connection from client!\n");
		ClientInfo newClient;
		newClient.socket = clientSocket;
		newClient.buffer = Buffer(128);
		newClient.connected = true;
		m_serverInfo.clients.push_back(newClient);
	}
}

/// <summary>
/// Get address information
/// </summary>
void AuthServer::GetServerAddrInfo()
{
	int state = -1;

	ZeroMemory(&m_serverInfo.hints, sizeof(m_serverInfo.hints));

	m_serverInfo.hints.ai_family = AF_INET;
	m_serverInfo.hints.ai_flags = AI_PASSIVE;
	m_serverInfo.hints.ai_protocol = IPPROTO_TCP;
	m_serverInfo.hints.ai_socktype = SOCK_STREAM;

	state = getaddrinfo(NULL, DEFAULT_PORT, &m_serverInfo.hints, &m_serverInfo.info);
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
/// Create socket
/// </summary>
void AuthServer::CreateSocket()
{
	m_serverInfo.listenSocket =
		socket(
			m_serverInfo.info->ai_family,
			m_serverInfo.info->ai_socktype,
			m_serverInfo.info->ai_protocol
		);
	if (m_serverInfo.listenSocket == INVALID_SOCKET) {
		printf("Failed to Create Socket. Error - %d\n", WSAGetLastError());
		freeaddrinfo(m_serverInfo.info);
		WSACleanup();
		exit(1);
	}
	else {
		printf("Socket was created successful!\n");
	}
}

/// <summary>
/// Startup WSAStartup
/// </summary>
void AuthServer::Startup()
{
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
