#pragma once
#include <link>

void CheckUserExist(Request* req, Response* res);
void GetUserPFP(Request* req, Response* res);
void UserLogin(Request* req, Response* res);
void UserCreate(Request* req, Response* res);

std::string GetUserID(std::string SID, std::string user);