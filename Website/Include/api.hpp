#pragma once
#include <link>

void CheckUserExist(Request* req, Response* res);
void GetUserPFP(Request* req, Response* res);
void UserLogin(Request* req, Response* res);
void UserCreate(Request* req, Response* res);

std::string GetUserID(std::string SID, std::string user);
std::vector<std::string> GetSessionUUIDs(std::string sessionID);

class UserInfo {
  public:
    UserInfo(std::string uuid);
    std::string GetUUID();
    std::string GetEmail();
    std::string GetPassword();
    std::string GetFirstName();
    std::string GetLastName();
    std::vector<std::string> GetPhoneNumbers();
  private:
    std::string uuid, email, password, firstname, lastname;
    std::vector<std::string> phonenumbers;
};

std::string AddUserInfo(std::string str, std::string uuid, std::string user, std::string cookies, std::string redir);