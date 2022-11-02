#include "Client.h"
#include "Buffer.h"
#include "Common.h"
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include "registration.pb.h"

#define BACKSPACE 8
#define ENTER 13
#define ESC 27

Client client;
std::string message = " ";
std::string name = "";


void SetConsoleColor(int);
void AuthenticateUser();
void RegisterOrLogin(int, std::string, std::string);

/// <summary>
/// Process user input
/// </summary>
/// <param name="mesg"></param>
void ProcessMessage(std::string mesg) {
	Buffer buf;
	if (mesg.find("Join") != std::string::npos) {
		std::string::size_type z = mesg.find("Join ");
		if (z != std::string::npos)
			mesg.erase(z, 5);
		JoinRoom joinRoomPkt;
		joinRoomPkt.messageId = 1;
		joinRoomPkt.roomName = mesg + " " + name.c_str();
		joinRoomPkt.packetLength = 
			sizeof(Header) + 
			sizeof(joinRoomPkt.roomName.size()) + 
			joinRoomPkt.roomName.size();

		buf = Buffer();
		buf.WriteUInt32(joinRoomPkt.packetLength);
		buf.WriteShort(joinRoomPkt.messageId);
		buf.WriteUInt32(joinRoomPkt.roomName.size());
		buf.WriteString((char*)joinRoomPkt.roomName.c_str());

		client.Send((const char*)(buf.Data.data()), joinRoomPkt.packetLength);
	}
	else if (mesg.find("Leave") != std::string::npos) {
		std::string::size_type z = mesg.find("Leave ");
		if (z != std::string::npos)
			mesg.erase(z, 6);
		LeaveRoom leaveRoomPkt;
		leaveRoomPkt.messageId = 2;
		leaveRoomPkt.roomName = mesg + " " + name.c_str();
		leaveRoomPkt.packetLength =
			sizeof(Header) +
			sizeof(leaveRoomPkt.roomName.size()) +
			leaveRoomPkt.roomName.size();

		buf = Buffer();
		buf.WriteUInt32(leaveRoomPkt.packetLength);
		buf.WriteShort(leaveRoomPkt.messageId);
		buf.WriteUInt32(leaveRoomPkt.roomName.size());
		buf.WriteString((char*)leaveRoomPkt.roomName.c_str());

		client.Send((const char*)(buf.Data.data()), leaveRoomPkt.packetLength);
	}
	else if (mesg.find("Send") != std::string::npos) {
		SendMessageData sendMessagePkt;
		sendMessagePkt.messageId = 3;
		sendMessagePkt.roomName = mesg.substr(5);
		sendMessagePkt.message = name;
		sendMessagePkt.packetLength =
			sizeof(Header) +
			sizeof(sendMessagePkt.roomName.size()) +
			sendMessagePkt.roomName.size() + 
			sizeof(sendMessagePkt.message.size()) +
			sendMessagePkt.message.size();

		buf = Buffer();
		buf.WriteUInt32(sendMessagePkt.packetLength);
		buf.WriteShort(sendMessagePkt.messageId);
		buf.WriteUInt32(sendMessagePkt.roomName.size());
		buf.WriteString((char*)sendMessagePkt.roomName.c_str());
		buf.WriteUInt32(sendMessagePkt.message.size());
		buf.WriteString((char*)sendMessagePkt.message.c_str());

		client.Send((const char*)(buf.Data.data()), sendMessagePkt.packetLength);
	}
	else {
		printf("Invalid message. Please use formats: 'Join roomname', 'Leave roomname' and 'Send roomname msg \n");
	}
}

/// <summary>
/// Keyboard input listener
/// </summary>
void ProcessKeyboardInput() {
	if (_kbhit) {
		int key = _getch();
		switch (key)
		{
		case BACKSPACE:
			if (!message.empty())
				message.pop_back();
			break;
		case ENTER:
			if (message.length() > 0) {
				ProcessMessage(message);
				message.clear();
				printf("\n");
			}
			break;
		case ESC:
			SetConsoleColor(15);
			exit(client.ShutDown());
		default:
			message += (char)key;
			printf("%c", (char)key);
			break;
		}
	}
}

/// <summary>
/// Change console color
/// </summary>
/// <param name=""></param>
void SetConsoleColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}


void AuthenticateUser() {
	int option;
	std::string email, password;

	printf("Please select from the options below: \n");
	printf("1. Register \n");
	printf("2. Login \n");
	printf("--> ");
	std::cin >> option;

	printf("Email: --> ");
	std::cin >> email;
	printf("Password: --> ");

	switch (option)
	{
	case 1:
		std::cin >> password;
		RegisterOrLogin(option, email, password);
		break;
	case 2:
		RegisterOrLogin(option, email, password);
		break;
	default:
		printf("Invalid option selected. Goodbye \n");
		break;
	}
}

/// <summary>
/// Registers or trys to login to the chat application
/// </summary>
/// <param name="type">Either Registration or Login</param>
/// <param name="email">The users email</param>
/// <param name="password">The password</param>
void RegisterOrLogin(int type, std::string email, std::string password) {
	Buffer buf;

	// In MessageType enum, Register = 4 and Login = 5
	// So we are going to add 3 to type selected.
	int messageId = type + 3;

	account::CreateAccountWeb user;
	user.set_requestid(0); // Set to 0 temporary, the ChatServer will change this to the socketId
	user.set_email(email);
	user.set_plaintextpassword(password);

	std::string serializedUser;
	user.SerializeToString(&serializedUser);

	Authentication auth;
	auth.messageId = messageId;
	auth.userData = serializedUser;

	auth.packetLength = sizeof(Header) + sizeof(auth.userData.size()); //+ auth.userData.size();
	std::cout << "sizeof(Header): " << sizeof(Header) << " sizeof(auth.userData.size()): " << sizeof(auth.userData.size()) << " auth.userData.size(): " << auth.userData.size() <<  std::endl;


	buf = Buffer();
	buf.WriteUInt32(auth.packetLength);
	buf.WriteUInt32(auth.messageId);
	//buf.WriteUInt32(auth.userData.size());
	//buf.WriteString((char*)auth.userData.c_str());
	//buf.Data.assign(auth.userData.begin(), auth.userData.end());
	buf.Data.insert(buf.Data.end(), auth.userData.begin(), auth.userData.end());

	client.Send((const char*)(buf.Data.data()), auth.packetLength);

	int oLength = buf.ReadUInt32(0);
	int pId = buf.ReadUInt32(4);
	//int usedsize = buf.ReadUInt32(8);
	std::string mesg(buf.Data.begin() + 8, buf.Data.end());
	std::cout << "oLength: " << oLength << " pId: " << pId << " mesg: " << mesg << " serializedUser: " << serializedUser << std::endl;

	account::CreateAccountWeb deserializeUser;
	bool success = deserializeUser.ParseFromString(mesg);
	if (!success) {
		std::cout << "Failed to parse user" << std::endl;
	}
	std::cout << deserializeUser.requestid() << std::endl;
	std::cout << deserializeUser.email() << std::endl;
	std::cout << deserializeUser.plaintextpassword() << std::endl;
}

int main(int argc, char* argv) {
	printf("##################################################################\n");
	printf("#                       DEMO CHAT                                #\n");
	printf("#               Enter username to start                          #\n");
	printf("#               Enter 'Join roomname' to Join room               #\n");
	printf("#               Enter 'Leave roomname' to Leave room             #\n");
	printf("#               Groups: general, staff, students (case sensitive)#\n");
	printf("##################################################################\n");
	
	const int recvBufLen = 128;
	char recvBuf[recvBufLen];

	client = Client();
	client.Initialize();

	int success = 0;

	if (client.Connect() != success)
		return 1;

	if (client.ManageSocket() != success)
		return 1;

	// Authenticate users before they login
	AuthenticateUser();

	SetConsoleColor(1);
	printf("\n");
	while (name.length() == 0) {
		printf("Please enter username: ");
		std::cin >> name;
	}
	while (true) {
		ProcessKeyboardInput();
		client.Receive(recvBuf, recvBufLen);
	}

	client.ShutDown();
	return 0;
}