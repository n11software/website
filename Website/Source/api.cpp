#include <api.hpp>
#include <db.hpp>
#include <utils.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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
    if (file.good()) {
      std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      res->Send(compress(str));
    } else {
      std::ifstream file("pfp/default.png");
      std::string defaultimg((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      res->Send(compress(defaultimg));
    }
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

std::string generateCode(int len, int seed) {
  srand(time(0)*rand()*seed);
  char hexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  std::string token;
  for (int i = 0; i < len; i++) token += hexChar[rand() % 10];
  return token;
}

#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <vmime/utility/stream.hpp>
#include <vmime/utility/inputStreamStringAdapter.hpp>
#include <vmime/security/cert/certificateChain.hpp>
#include <tracer.hpp>
#include <authenticator.hpp>
#include <certificateVerifier.hpp>
#include <timeoutHandler.hpp>
static vmime::shared_ptr <vmime::net::session> g_session = vmime::net::session::create();
static void sendMessage(std::string code, std::string email) {

	try {

		// Request user to enter an URL
    std::ifstream file("2fa.txt");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string pass;
    for (std::string line: split(str, "\n")) {
      if (line.find("SMTP_PASS=") != std::string::npos) {
        pass = line.substr(10);
      }
    }
		vmime::utility::url url("smtp", "mail.n11.dev", 587, "", "2fa@n11.dev", pass);
		vmime::shared_ptr <vmime::net::transport> tr;

    tr = g_session->getTransport(url);

#if VMIME_HAVE_TLS_SUPPORT
		tr->setProperty("connection.tls", true);
		tr->setTimeoutHandlerFactory(vmime::make_shared <timeoutHandlerFactory>());
		tr->setCertificateVerifier(
			vmime::make_shared <interactiveCertificateVerifier>()
		);

#endif // VMIME_HAVE_TLS_SUPPORT
		if (!url.getUsername().empty() || !url.getPassword().empty()) {
			tr->setProperty("options.need-authentication", true);
		}
		vmime::shared_ptr <std::ostringstream> traceStream = vmime::make_shared <std::ostringstream>();
		tr->setTracerFactory(vmime::make_shared <myTracerFactory>(traceStream));
		vmime::mailbox from("2fa@n11.dev");
		vmime::mailboxList to;
    to.appendMailbox(vmime::make_shared <vmime::mailbox>(email));
		std::string data = "Your code is " + code;
		tr->connect();
		vmime::string msgData = data;
		vmime::utility::inputStreamStringAdapter vis(msgData);
		tr->send(from, to, vis, msgData.length());
		tr->disconnect();

	} catch (vmime::exception& e) {
		throw e;
	} catch (std::exception& e) {
		std::cerr << std::endl;
		std::cerr << "std::exception: " << e.what() << std::endl;
		throw;
	}
}

#include <twilio.hpp>
void sendSMS(std::string code, std::string number) {
  std::string res;
  std::ifstream file("2fa.txt");
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  std::string api, key;
  for (std::string line: split(str, "\n")) {
    if (line.find("TWILIO=") != std::string::npos) {
      api = line.substr(7);
    } else if (line.find("TWILIO_KEY=") != std::string::npos) {
      key = line.substr(11);
    }
  }
  auto twilio = std::make_shared<twilio::Twilio>(api, key);
  twilio->send_message(number, "8176702119", "Your N11 verification code is: " + code, res, "", false);
}

int CreateSession(Request* req, Response* res, std::string uuid) {
  std::string token = generateUUID(16, req->GetSocket());
  sql::Statement* stmt = getConnection()->createStatement();
  stmt->execute("INSERT INTO tokens (id, uuid, ip, browser, os, expired, date) VALUES ('" + token + "', '" + uuid + "', '" + (req->GetHeader("x-forwarded-for")==""?"Unknown":req->GetHeader("x-forwarded-for")) + "', '" + req->GetHeader("user-agent").substr(0, req->GetHeader("user-agent").length()-1) + "', '" + req->GetFormParam("os") + "', '0', '" + std::to_string(time(NULL)) + "')");
  std::vector<std::string> cookies = split(req->GetHeader("cookie"), "; ");
  std::string session = "";
  for (std::string cookie: cookies) {
    if (cookie[cookie.length()-1] == '\r') cookie = cookie.substr(0, cookie.length()-1);
    if (cookie.substr(0,8) == "session=") {
      session = cookie.substr(8);
      break;
    }
  }
  int i = 0;
  if (session != "") {
    sql::ResultSet* rs2 = stmt->executeQuery("SELECT * FROM sessions WHERE id = '" + session + "'");
    if (rs2->next()) {
      stmt->execute("UPDATE sessions SET tokens = '"+rs2->getString("tokens")+";"+token+"' WHERE id = '" + session + "'");
      i = split(rs2->getString("tokens"), ";").size()-1;
    } else {
      session = generateUUID(32, req->GetSocket());
      stmt->execute("INSERT INTO sessions (id, tokens) VALUES ('" + session + "', '" + token + "')");
    }
  } else {
    session = generateUUID(32, req->GetSocket());
    stmt->execute("INSERT INTO sessions (id, tokens) VALUES ('" + session + "', '" + token + "')");
  }
  res->SetHeader("set-cookie", "session=" + session + "; expires=Fri, 31 Dec 9999 23:59:59 GMT; path=/");
  return i;
}

void UserLogin(Request* req, Response* res) {
  res->SetHeader("Content-Type", "application/json; charset=utf-8");
  res->SetHeader("Content-Encoding", "gzip");
  sql::Statement* stmt = getConnection()->createStatement();
  sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM accounts WHERE email = '" + req->GetFormParam("email") + "' AND password = '" + SHA512::hash(req->GetFormParam("password")) + "'");
  if (rs->next()) {
    if (rs->getBoolean("2fa")==1) {
      if (req->GetFormParam("requestCode") != "") {
        // Send email or sms
        std::string code = generateCode(4, req->GetSocket());
        getConnection()->createStatement()->execute("UPDATE accounts SET 2facode = '" + code + "' WHERE uuid = '" + rs->getString("uuid") + "'");
        if ((rs->getString("2fat") == "e" || rs->getString("2fat") == "b") && req->GetFormParam("requestCode") == "0") sendMessage(code, req->GetFormParam("email"));
        else if ((rs->getString("2fat") == "p" || rs->getString("2fat") == "b") && req->GetFormParam("requestCode") != "0") {
          int x = atoi(req->GetFormParam("requestCode").c_str());
          if (rs->getString("2fat") == "b") x--;
          std::string phone = split(rs->getString("phonenumbers"),";")[x];
          sendSMS(code, phone);
        }
        res->Send(compress("sending"));
      } else if (req->GetFormParam("code")=="") {
        // Show emails and phones
        int i=0;
        std::string ret = "{\"FA\":[";
        if (rs->getString("2fat") == "p" || rs->getString("2fat") == "b") {
          std::vector<std::string> factors = split(rs->getString("phonenumbers"), ";");
          if (rs->getString("2fat") == "b") {
            ret+= "\"" + rs->getString("email") + "\",";
            i++;
          }
          for (std::string factor: factors) {
            ret += "\"" + factor + "\",";
            i++;
          }
        } else if (rs->getString("2fat") == "e") {
          ret+= "\"" + rs->getString("email") + "\",";
          i++;
        }
        if (i>0) ret = ret.substr(0, ret.length()-1) + "]}";
        res->Send(compress(ret));
      } else if (req->GetFormParam("code") != "") {
        // Check code
        if (req->GetFormParam("code") == rs->getString("2facode")) {
          CreateSession(req, res, rs->getString("uuid"));
          res->Send(compress("{\"success\": true}"));
        } else {
          res->Send(compress("{\"error\":\"Invalid code\"}"));
        }
      }
    } else {
      // Login
      int user = CreateSession(req, res, rs->getString("uuid"));
      res->Send(compress("{\"redir\": \"/?u=" + std::to_string(user) + "\"}"));
    }
  } else {
    res->SetStatus("401 Unauthorized");
    res->Send(compress("{\"error\":\"Wrong email or password\"}"));
  }
}

void UserCreate(Request* req, Response* res) {
  res->SetHeader("Content-Type", "application/json; charset=utf-8");
  res->SetHeader("Content-Encoding", "gzip");
  sql::Statement* stmt = getConnection()->createStatement();
  sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM accounts WHERE email = '" + req->GetFormParam("email") + "'");
  if (rs->next()) {
    res->SetStatus("401 Unauthorized");
    res->Send(compress("{\"error\":\"Email already exists\"}"));
  } else {
    if (req->GetFormParam("code") != "") {
      // Check if code is valid
      rs = stmt->executeQuery("SELECT * FROM code WHERE email = '" + req->GetFormParam("email") + "' AND code = '" + req->GetFormParam("code") + "'");
      if (rs->next()) {
        if (req->GetFormParam("password") != "") {
          // Create account
          std::string uuid = generateUUID(32, req->GetSocket());
          stmt->execute("INSERT INTO accounts (uuid, email, firstname, lastname, password, 2fa, 2fat, 2facode) VALUES ('" + uuid + "', '" + req->GetFormParam("email") + "', '"+req->GetFormParam("first")+"', '"+req->GetFormParam("last")+"', '" + SHA512::hash(req->GetFormParam("password")) + "', 0, '', '')");
          int z = CreateSession(req, res, uuid);
          res->Send(compress("{\"user\": "+std::to_string(z)+"}"));
        } else res->Send(compress("{\"success\": true}"));
      } else {
        res->SetStatus("401 Unauthorized");
        res->Send(compress("{\"error\":\"Invalid code\"}"));
      }
    } else {
      std::string code = generateCode(4, req->GetSocket());
      stmt = getConnection()->createStatement();
      rs = stmt->executeQuery("SELECT * FROM code WHERE email = '" + req->GetFormParam("email") + "'");
      if (rs->next()) {
        stmt->execute("UPDATE code SET code = '" + code + "' WHERE email = '" + req->GetFormParam("email") + "'");
      } else {
        stmt->execute("INSERT INTO code (email, code) VALUES ('" + req->GetFormParam("email") + "', '" + code + "')");
      }
      sendMessage(code, req->GetFormParam("email"));
      res->Send(compress("sending"));
    }
  }
}