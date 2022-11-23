#include "registration.pb.h"
#include "AuthServer.h"
#include "Database.h"


int main(int argc, char** argv) {
	int state = -1;
	//Database db = Database();
	printf("Successfully retrieved our ccp-conn-sql driver! -3\n");
	//db.Connect();
	//db.CreateUser("tests1", "test123");
	//db.Login("tests1", "test1234");
	//db.Disconnect();
	AuthServer server = AuthServer();
	printf("Successfully retrieved our ccp-conn-sql driver! 1\n");
	server.Initialize();
	printf("Successfully retrieved our ccp-conn-sql driver! 2\n");
	state = server.Bind();
	printf("Successfully retrieved our ccp-conn-sql driver! 3\n");
	state = server.Listen();
	printf("Successfully retrieved our ccp-conn-sql driver! 4\n");
	server.Process();
	printf("Successfully retrieved our ccp-conn-sql driver! 5\n");
	server.ShutDown();
	printf("Successfully retrieved our ccp-conn-sql driver! 6\n");
	/*account::CreateAccountWeb user;
	user.set_requestid(0);
	user.set_email("ademola.adedeji@hotmail.com");
	user.set_plaintextpassword("password");

	std::string userString;
	user.SerializeToString(&userString);

	std::cout << userString << std::endl;

	for (int idxString = 0; idxString < userString.length(); idxString++) {
		printf("%02X ", userString[idxString]);
	}
	printf("\n");

	account::CreateAccountWeb deserializeUser;
	bool success = deserializeUser.ParseFromString(userString);
	if (!success) {
		std::cout << "Failed to parse user" << std::endl;
	}
	std::cout << deserializeUser.email() << std::endl;*/
	return 0;
}