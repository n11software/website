#include <API.hpp>

void Exists(Link::Request* request, Link::Response* response) {
    response->SetBody("{\"exists\":true}");
}

API::API(Link::Server* server) {
    this->server = server;
    server->Get("api/user/exists", Exists);
}