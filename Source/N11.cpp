#include <iostream>
#include <Link.hpp>
#include <thread>
#include <fstream>

#define PRODUCTION true
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

int main() {
    Link::Server server;

    server.SetStaticPages("public/");

    server.Get("/", [](Link::Request* request, Link::Response* response) {
        std::ifstream file("pages/index.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        response->SetBody(content);
    });

    server.Error(404, [](Link::Request* request, Link::Response* response) {
        response->SetStatus(404)->SetBody("404 Not Found");
    });

    #if PRODUCTION == true
        server.EnableSSL(CERT, KEY);
        std::thread http(HTTPThread);
        http.detach();
    #endif

    std::cout << "Server started!" << std::endl;
    server.Start();
    return 0;
}