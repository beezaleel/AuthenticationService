#include "registration.pb.h"
#include "AuthServer.h"
#include "Database.h"


int main(int argc, char** argv) {
	int state = -1;
	AuthServer server = AuthServer();
	server.Initialize();
	state = server.Bind();
	state = server.Listen();
	server.Process();
	server.ShutDown();
	
	return 0;
}