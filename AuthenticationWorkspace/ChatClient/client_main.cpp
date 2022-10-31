#include "Client.h"
#include "Buffer.h"
#include "Common.h"
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>

#define BACKSPACE 8
#define ENTER 13
#define ESC 27

Client client;
std::string message = " ";
std::string name = "";


void SetConsoleColor(int);

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