#include <iostream>
#include <Link.hpp>
#include <thread>
#include <fstream>
#include <API.hpp>

#define PRODUCTION false
#define CERT "certificate.pem"
#define KEY "key.pem"

void RedirectHTTPS(Link::Request* request, Link::Response* response) {
    response->SetStatus(301)->SetHeader("Location", "https://" + request->GetHeader("Host") + request->GetPath());
}

void HTTPThread() {
    Link::Server http;
    http.Error(404, RedirectHTTPS);
    http.Start();
}

std::string replace(std::string data, std::string delimiter, std::string replacement) {
  size_t pos = 0;
  while ((pos = data.find(delimiter)) != std::string::npos) {
    data.replace(pos, delimiter.length(), replacement);
  }
  return data;
}


int main() {
    Link::Server server;

    server.SetStaticPages("public/");

    server.Get("/", [](Link::Request* request, Link::Response* response) {
        std::string id = request->GetCookie("id");
        if (id.empty()) {
            response->SetStatus(301)->SetHeader("Location", "/login");
            return;
        }
        std::ifstream file("pages/index.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response->SetBody(content);
    });

    server.Get("/login", [](Link::Request* request, Link::Response* response) {
        std::string id = request->GetCookie("id");
        std::ifstream file("pages/login.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (id.empty()) {
            content = replace(content, "[nothidden]", "hidden");
            content = replace(content, "[hidden]", "");
            content = replace(content, "[acclist]", "");
        } else {
            content = replace(content, "[nothidden]", "");
            content = replace(content, "[hidden]", "hidden");
            std::string accList = "";
            content = replace(content, "[acclist]", accList);
        }
        response->SetBody(content);
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
    #endif

    std::cout << "Server started!" << std::endl;
    server.Start();
    return 0;
}