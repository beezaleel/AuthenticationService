#pragma once
#include <string>

#ifdef CHATLIB_EXPORTS
#define CHATLIB_EXPORTS_API __declspec(dllexport)
#else
#define CHATLIB_EXPORTS_API __declspec(dllimport)
#endif

struct Header {
	int packetLength;
	int messageId;
};

struct SendMessageData : public Header {
	std::string roomName;
	std::string message;
};

struct JoinRoom : public Header {
	std::string roomName;
};

struct LeaveRoom : public Header {
	std::string roomName;
};
