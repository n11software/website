#pragma once
#include <Link.hpp>
#include <json.hpp>

class API {
    public:
        API(Link::Server* server);
    private:
        Link::Server* server;
};

nlohmann::json sql(std::string SQL);