#include <iostream>
#include <Link.hpp>
#include <thread>
#include <fstream>
#include <API.hpp>
#include <json.hpp>
#include <User.hpp>

#define PRODUCTION false
#define CERT "cert.pem"
#define KEY "key.pem"

void RedirectHTTPS(Link::Request* request, Link::Response* response, Link::Server* server) {
    response->SetStatus(301)->SetHeader("Location", "https://" + request->GetHeader("Host") + request->GetPath())->Close();
}

void HTTPThread() {
    Link::Server http;
    http.Use(RedirectHTTPS);
    http.Start();
}

std::string replace(std::string data, std::string delimiter, std::string replacement) {
  size_t pos = 0;
  while ((pos = data.find(delimiter)) != std::string::npos) {
    data.replace(pos, delimiter.length(), replacement);
  }
  return data;
}

bool isLoggedIn(Link::Request* request, Link::Response* response, nlohmann::json& json) {
    Link::Request* req = new Link::Request("http://localhost:8000/sql");
    req->SetMethod("POST");
    std::string id = request->GetCookie("id");
    if (id.empty()) return false;
    else {
        for (int i = 0; i < id.length(); i++) {
            switch (id[i]) {
                case '0'...'9':
                case 'a'...'z':
                case 'A'...'Z':
                    break;
                default:
                    return false;
            }
        }
    }
    req->SetBody("SELECT Tokens.Account.*, Tokens.Active, Tokens.Time, IP, Location, UserAgent, id FROM Session WHERE id='Session:"+request->GetCookie("id")+"';");
    req->SetRawHeader("Authorization", "Basic cm9vdDpyb290");
    req->SetRawHeader("Accept", "application/json");
    req->SetRawHeader("NS", "N11");
    req->SetRawHeader("DB", "N11");
    req->SetRawHeader("Content-Type", "application/text");
    req->SetRawHeader("Content-Length", std::to_string(req->GetBody().length()));
    Link::Client client(req);
    Link::Response* res = client.Send();
    std::cout << res->GetBody() << std::endl;
    json = nlohmann::json::parse(res->GetBody());
    if (json[0]["result"].empty()) return false;
    return true;
}

bool isLoggedIn(Link::Request* request, Link::Response* response) {
    nlohmann::json json;
    return isLoggedIn(request, response, json);
}

int main() {
    Link::Server server;

    server.SetStaticPages("public/");

    server.Get("/", [](Link::Request* request, Link::Response* response) {
        if (!isLoggedIn(request, response)) {
            response->SetStatus(302)->SetHeader("Location", "/login");
            return;
        }
        std::ifstream file("pages/index.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response->SetBody(content);
    });

    server.Get("/signup", [](Link::Request* request, Link::Response* response) {
        std::ifstream file("pages/signup.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response->SetBody(content);
    });

    server.Get("/login", [](Link::Request* request, Link::Response* response) {
        nlohmann::json json;
        bool loggedIn = isLoggedIn(request, response, json);
        std::string id = request->GetCookie("id");
        std::ifstream file("pages/login.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        content = replace(content, "[ip]", request->GetIP());
        content = replace(content, "[user-agent]", loggedIn?std::string(json[0]["result"][0]["Location"]):request->GetHeader("User-Agent"));
        response->SetBody(content);
    });

    server.Get("/logout", [](Link::Request* request, Link::Response* response) {
        response->SetHeader("Set-Cookie", "id=; expires=Thu, 01 Jan 1970 00:00:00 GMT");
        response->SetStatus(302)->SetHeader("Location", "/login");
    });

    API api(&server);

    server.Error(404, [](Link::Request* request, Link::Response* response) {
        response->SetStatus(404)->SetBody("404 Not Found");
    });

    #if PRODUCTION == true
        server.EnableSSL(CERT, KEY);
        std::thread http(HTTPThread);
        http.detach();
    #else
        server.SetPort(8080);
        server.EnableDebugging();
    #endif

    server.EnableMultiThreaded();
    server.SetStartMessage("Server started on port " + std::to_string(server.GetPort()) + "!");
    server.EnableDebugging();
    server.Start();
    return server.Status;
}
