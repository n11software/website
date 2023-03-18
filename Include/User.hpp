#pragma once
#include <string>
#include <vector>

struct User {
    std::string Email, Created, Password;
    bool MFA, SMS, Authenticator;
    std::vector<std::string> PhoneNumbers, Emails;
    std::string ID;
};

struct Error {
    std::string Message;
    int Code;
};

Error CreateUser(User& user);