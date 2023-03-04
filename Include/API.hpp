#pragma once
#include <Link.hpp>

class API {
    public:
        API(Link::Server* server);
    private:
        Link::Server* server;
};