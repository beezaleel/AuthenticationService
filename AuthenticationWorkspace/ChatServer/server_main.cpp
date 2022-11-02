#include <iostream>
#include "Buffer.h"
#include "Common.h"
#include "Server.h"
#include <string>

int main(int argc, char* argv) {
	int state = -1;
	Server server = Server();
	server.Initialize();
	state = server.Bind();
	state = server.Listen();
	server.Process();
	server.ShutDown();
	return 0;
}