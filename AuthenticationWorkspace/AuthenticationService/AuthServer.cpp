#include "AuthServer.h"
#include "registration.pb.h"
#include "login.pb.h"
#include <Common.h>

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

	// Database initialization
	m_database = Database();
	m_database.Connect();
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
			unsigned int messageId = client.buffer.ReadShort(4);
			account::CreateAccountWeb deserializeUser;
			unsigned int messageLength = client.buffer.ReadUInt32(8);
			User refUser;



			std::string serializedUser(client.buffer.Data.begin() + 12, client.buffer.Data.begin() + messageLength + 12);
			bool result;
			int loginResult;

			switch (messageId)
			{
			case MessageType::Register:
				result = deserializeUser.ParseFromString(serializedUser);
				if (!result) {
					std::cout << "Failed to parse user" << std::endl;
				}
				result = m_database.CreateUser(deserializeUser.email(), deserializeUser.plaintextpassword());
				break;
			case MessageType::Login:
				result = deserializeUser.ParseFromString(serializedUser);
				if (!result) {
					std::cout << "Failed to parse user" << std::endl;
				}
				loginResult = m_database.Login(deserializeUser.email(), deserializeUser.plaintextpassword(), refUser);
				LoginResponse(loginResult, refUser);
				break;
			default:
				break;
			}
		}
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
int AuthServer::Send(ClientInfo& client, char buf[], int bufLen)
{
	int state = -1;
	state = send(client.socket, buf, bufLen, 0);
	if (state == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(client.socket);
		WSACleanup();
		return 1;
	}

	return state;
}

/// <summary>
/// Shut down socket
/// </summary>
void AuthServer::ShutDown()
{
	printf("Shutting down server . . .\n");
	m_database.Disconnect();
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

/// <summary>
/// The response sent back to Chat server
/// </summary>
/// <param name="result">The database result</param>
void AuthServer::LoginResponse(int result, User user)
{
	time_t rawtime;
	struct tm ltm;
	time(&rawtime);
	localtime_s(&ltm, &rawtime);
	std::ostringstream datetimeString;
	datetimeString << std::put_time(&ltm, "%Y-%m-%d %H:%M:%S");

	Authentication auth;
	auth.messageId = MessageType::Login;

	Buffer buf = Buffer();

	if (result == 1) {
		account::AuthenticateWebSuccess response;

		response.set_requestid(0);
		response.set_userid(user.userId);
		response.set_creationdate(datetimeString.str());

		std::string serializedResponse;
		response.SerializeToString(&serializedResponse);
		auth.userData = serializedResponse;
		auth.packetLength = sizeof(Header) + sizeof(auth.userData.size()) + auth.userData.size();

		buf.WriteUInt32(auth.packetLength);
		buf.WriteUInt32(auth.messageId);
		buf.WriteUInt32(auth.userData.size());
		buf.Data.insert(buf.Data.end(), auth.userData.begin(), auth.userData.end());

		Send(m_serverInfo.clients[0], (char*)(buf.Data.data()), auth.packetLength);
	}
	else {
		account::AuthenticateWebFailure response;

		response.set_requestid(0);
		if (result == -3)
			response.set_failurereason(account::AuthenticateWebFailure_reason_INVALID_CREDENTIALS);
		else
			response.set_failurereason(account::AuthenticateWebFailure_reason_INTERNAL_SERVER_ERROR);

		std::string serializedResponse;
		response.SerializeToString(&serializedResponse);
		auth.userData = serializedResponse;
		auth.packetLength = sizeof(Header) + sizeof(auth.userData.size()) + auth.userData.size();

		buf.WriteUInt32(auth.packetLength);
		buf.WriteUInt32(auth.messageId);
		buf.WriteUInt32(auth.userData.size());
		buf.Data.insert(buf.Data.end(), auth.userData.begin(), auth.userData.end());

		Send(m_serverInfo.clients[0], (char*)(buf.Data.data()), auth.packetLength);
	}
}
