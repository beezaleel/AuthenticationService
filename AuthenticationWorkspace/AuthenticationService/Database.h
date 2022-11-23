#pragma once
#include <string>
#include "jdbc/mysql_driver.h"
#include "jdbc/mysql_connection.h"
#include "jdbc/mysql_error.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "SHA256.h"

struct User {
	std::string email;
	std::string password;
	std::string salt;
	std::string hashPassword;
};

struct DatabaseCredentials {
	std::string hostname;
	std::string username;
	std::string password;
};

class Database
{
public:
	Database();
	~Database();
	bool Connect();
	int CreateUser(std::string email, std::string password);
	int Login(std::string email, std::string password);
	void Disconnect();

private:
	sql::Driver* mPDriver;
	sql::Connection* mPConnection;
	sql::ResultSet* mPResultSet;
	sql::Statement* mPStatement;
	sql::PreparedStatement* mPInsertStatement;
	User mUser;
	bool UserExist(std::string email);
	std::string GenerateSalt();
};
