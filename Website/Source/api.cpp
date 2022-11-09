#include <api.hpp>
#include <db.hpp>
#include <utils.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <SHA512>

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

std::string generateUUID(int len, int seed) {
  srand(time(0)*rand()*seed);
  char hexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  std::string token;
  for (int i = 0; i < len; i++) token += hexChar[rand() % 16];
  return token;
}

void UserLogin(Request* req, Response* res) {
  res->SetHeader("Content-Type", "application/json; charset=utf-8");
  res->SetHeader("Content-Encoding", "gzip");
  sql::Statement* stmt = getConnection()->createStatement();
  sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM accounts WHERE email = '" + req->GetFormParam("email") + "' AND password = '" + SHA512::hash(req->GetFormParam("password")) + "'");
  if (rs->next()) {
    if (rs->getBoolean("2fa")) {
      res->Send(compress("{\"FA\":[\"8066628337\", \"levicowan2005@icloud.com\"]}"));
    } else {
      // Login
      std::string token = generateUUID(16, req->GetSocket());
      stmt->execute("INSERT INTO tokens (id, uuid, ip, browser, os, expired, date) VALUES ('" + token + "', '" + rs->getString("uuid") + "', '" + (req->GetHeader("x-forwarded-for")==""?"Unknown":req->GetHeader("x-forwarded-for")) + "', '" + req->GetHeader("user-agent").substr(0, req->GetHeader("user-agent").length()-1) + "', '" + req->GetFormParam("os") + "', '0', '" + std::to_string(time(NULL)) + "')");
      std::vector<std::string> cookies = split(req->GetHeader("cookie"), "; ");
      std::string session = "";
      for (std::string cookie: cookies) {
        if (cookie[cookie.length()-1] == '\r') cookie = cookie.substr(0, cookie.length()-1);
        if (cookie.substr(0,8) == "session=") {
          session = cookie.substr(8);
          break;
        }
      }
      if (session != "") {
        sql::ResultSet* rs2 = stmt->executeQuery("SELECT * FROM sessions WHERE id = '" + session + "'");
        rs2->next();
        stmt->execute("UPDATE sessions SET tokens = '"+rs2->getString("tokens")+";"+token+"' WHERE id = '" + session + "'");
      } else {
        session = generateUUID(32, req->GetSocket());
        stmt->execute("INSERT INTO sessions (id, tokens) VALUES ('" + session + "', '" + token + "')");
      }
      res->SetHeader("set-cookie", "session=" + session + "; expires=Fri, 31 Dec 9999 23:59:59 GMT; path=/");
      res->Send(compress("{\"FA\":[\"8066628337\", \"levicowan2005@icloud.com\"]}"));
    }
  } else {
    res->SetStatus("401 Unauthorized");
    res->Send(compress("{\"error\":\"Wrong email or password\"}"));
  }
}