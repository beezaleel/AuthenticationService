#include <iostream>
#include "Buffer.h"
#include "Common.h"
#include "Server.h"
#include <string>

int main(int argc, char* argv) {
	/*Header h;
	h.messageId = 10;
	h.packetLength = 20;

	std::cout << "Message Id: " << h.messageId << " Packet length: " << h.packetLength << std::endl;

	Buffer uIntTest = Buffer(2);
	uIntTest.WriteUInt32(4294967294);
	uint32_t g = uIntTest.ReadUInt32(0);
	std::cout << "Assert: 4294967295 equals: " << g << std::endl;

	Buffer signedIntTest = Buffer(2);
	signedIntTest.WriteSignedInt(-21474836);
	int x = signedIntTest.ReadSignedInt(0);
	std::cout << "Assert: -21474836 equals: " << x << std::endl;

	Buffer signedIntTest2 = Buffer(2);
	signedIntTest2.WriteSignedInt(2147483);
	int a = signedIntTest2.ReadSignedInt(0);
	std::cout << "Assert: 2147483 equals: " << a << std::endl;

	Buffer shortTest = Buffer(2);
	shortTest.WriteShort(-22768);
	int b = shortTest.ReadShort(0);
	std::cout << "Assert: -22768 equals: " << b << std::endl;

	Buffer shortTest2 = Buffer(2);
	shortTest2.WriteShort(22767);
	int c = shortTest2.ReadShort(0);
	std::cout << "Assert: 22767 equals: " << c << std::endl;
	
	char names[] = "?><()*&^%$#@!{}:";
	Buffer stringTest = Buffer();
	stringTest.WriteString(names);
	char* result = stringTest.ReadString(0);
	std::cout << "Assert serialized:" << names << " equals Deserialized: " << result << std::endl;*/

	int state = -1;
	Server server = Server();
	server.Initialize();
	state = server.Bind();
	state = server.Listen();
	server.Process();
	server.ShutDown();
	return 0;
}