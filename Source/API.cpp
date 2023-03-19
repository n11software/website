#include <API.hpp>
#include <regex>

void Exists(Link::Request* request, Link::Response* response) {
    // validate email
    std::string email = request->GetParam("email");
    if (email.empty()) {
        response->SetBody("{\"error\":\"Email is required\"}");
        return;
    }

    // check if email is valid

    if (!std::regex_match(email, std::regex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$"))) response->SetBody("{\"error\":\"Email is invalid\"}");

    nlohmann::json json = sql("SELECT * FROM Users WHERE Email='"+email+"';");
    if (json.empty()) response->SetBody("{\"exists\":false}");
    else response->SetBody("{\"exists\":true}");
}

API::API(Link::Server* server) {
    this->server = server;
    server->Get("api/user/exists", Exists);
}

nlohmann::json sql(std::string SQL) {
    Link::Request* req = new Link::Request("http://localhost:8000/sql");
    req->SetMethod("POST");
    req->SetBody(SQL);
    req->SetRawHeader("Authorization", "Basic cm9vdDpyb290");
    req->SetRawHeader("Accept", "application/json");
    req->SetRawHeader("NS", "N11");
    req->SetRawHeader("DB", "N11");
    req->SetRawHeader("Content-Type", "application/text");
    req->SetRawHeader("Content-Length", std::to_string(req->GetBody().length()));
    Link::Client client(req);
    Link::Response* res = client.Send();
    return nlohmann::json::parse(res->GetBody())[0]["result"];
}