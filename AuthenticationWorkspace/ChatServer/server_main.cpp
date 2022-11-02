#include <iostream>
#include "Buffer.h"
#include "Common.h"
#include "Server.h"
#include <string>

int main(int argc, char* argv) {
	int state = -1;
	Server server = Server();
	Server authServer = Server();
	server.Initialize();
	authServer.ConnectToAuthServer("9000");
	state = server.Bind();
	state = server.Listen();
	server.Process();
	server.ShutDown();
	return 0;
}