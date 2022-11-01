#include "registration.pb.h"


int main(int argc, char** argv) {
	account::CreateAccountWeb user;
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
	std::cout << deserializeUser.email() << std::endl;
	return 0;
}