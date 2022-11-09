#include <api.hpp>
#include <db.hpp>
#include <utils.hpp>
#include <string>
#include <iostream>
#include <fstream>

void CheckUserExist(Request* req, Response* res) {
    res->SetHeader("Content-Type", "application/json; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    sql::Statement* stmt = getConnection()->createStatement();
    sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM accounts WHERE email = '" + req->GetQuery("email") + "'");
    if (rs->next()) {
      res->Send(compress("{\"exists\":true}"));
    } else {
      res->Send(compress("{\"exists\":false}"));
    }
}

void GetUserPFP(Request* req, Response* res) {
  res->SetHeader("Content-Type", "image/png");
  sql::Statement* stmt = getConnection()->createStatement();
  sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM accounts WHERE email = '" + req->GetQuery("email") + "'");
  if (rs->next()) {
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("pfp/" + rs->getString("uuid") + ".png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    res->Send(compress(str));
  } else {
    res->Error(404);
  }
}