#include "Database.h"
#include <exception>

Database::Database() : 
	mPDriver(nullptr), 
	mPConnection(nullptr),
	mPResultSet(nullptr), 
	mPStatement(nullptr),
	mPInsertStatement(nullptr)
{
}

Database::~Database()
{
}

bool Database::Connect()
{
	// Connection call here
	try {
		printf("Trying to connect to database server!\n");
		mPDriver = sql::mysql::get_driver_instance();
	}
	catch (sql::SQLException e) {
		printf("Failed to retrieve driver instance: %s query: %s\n", e.what(), e.getSQLState().c_str());
		return false;
	}
	catch (std::exception& e) {
		printf("Reason for failure! %s\n", e.what());
	}
	printf("Successfully retrieved our ccp-conn-sql driver!\n");

	try {
		sql::SQLString hostName("127.0.0.1:3306");
		sql::SQLString username("test");
		sql::SQLString password("password");

		mPConnection = mPDriver->connect(hostName, username, password);
		mPConnection->setSchema("auth");
	}
	catch (sql::SQLException e) {
		printf("Failed to connect to  Schema: %s query: %s\n", e.what(), e.getSQLState().c_str());
		return false;
	}
	printf("Successfully connected to auth Schema!\n");

	return true;
}

int Database::CreateUser(std::string email, std::string password)
{
	// Check if user exist
	if (UserExist(email))
		return -1;

	// create salt for the password
	std::string salt = GenerateSalt();

	// Password and Salt
	std::string saltPassword = salt + password;
	std::string hash = sha256(saltPassword);

	//time_t rawtime;
	//struct tm ltm;
	//time(&rawtime);
	//localtime_s(&ltm, &rawtime);
	//std::ostringstream ss;
	//ss << std::put_time(&ltm, "%Y-%m-%d %H:%M:%S");


	try {
		mPStatement = mPConnection->createStatement();
		mPInsertStatement = mPConnection->prepareStatement(
			"INSERT INTO web_auth (email, salt, hashed_password) VALUES (?, ?, ?);");
		mPInsertStatement->setString(1, email);
		mPInsertStatement->setString(2, salt.c_str());
		mPInsertStatement->setString(3, hash.c_str());

		mPInsertStatement->execute();
		return 1;
	}
	catch (sql::SQLException e) {
		printf("Failed to add a user to database: %s\n", e.what());
		return -1;
	}
	printf("Successfully added user to Database!\n");
}

int Database::Login(std::string email, std::string password)
{
	// Check if user exist
	if (!UserExist(email))
		return -1;
	std::string saltProvidedPassword = mUser.salt + password;
	std::string hashProvidedPassword = sha256(saltProvidedPassword);
	if (mUser.hashPassword != hashProvidedPassword)
		return -2;

	printf("hashProvidedPassword %s\n", hashProvidedPassword.c_str());
	return 0;
}

bool Database::UserExist(std::string email)
{
	try {
		mPStatement = mPConnection->createStatement();
		mPInsertStatement = mPConnection->prepareStatement("SELECT * FROM web_auth where email=?");
		mPInsertStatement->setString(1, email);
		mPResultSet = mPInsertStatement->executeQuery();

		mUser = User();
		while (mPResultSet->next()) {
			mUser.email = mPResultSet->getString("email");
			mUser.hashPassword = mPResultSet->getString("hashed_password");
			mUser.salt = mPResultSet->getString("salt");
		}
	}
	catch (sql::SQLException e) {
		printf("Failed to query our database: %s\n", e.what());
		return false;
	}
	printf("Successfully retrieved %d rows from the database!\n", (int)mPResultSet->rowsCount());

	return (int)mPResultSet->rowsCount();
}

void Database::Disconnect()
{
	try {
		mPConnection->close();
	}
	catch (sql::SQLException e) {
		printf("Failed to close the connection to database: %s errorCode: %d \n", e.what(), e.getErrorCode());
		return;
	}
	printf("Successfully closed the connection to our Database!\n");

	delete mPStatement;
	delete mPResultSet;
	delete mPInsertStatement;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::string Database::GenerateSalt()
{
	srand(time(NULL));
	std::string salt = "";
	unsigned char value;
	unsigned int min, max, range;
	min = 48;
	max = 84;
	range = max - min + 1;

	for (int i = 0; i < 10; i++) {
		value = rand() % range + min;
		salt += value;
	}

	return salt;
}
