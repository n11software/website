#include <link>

int main() {
  Link Server(3000);

  // Return 404 Page
  Server.Error(404, [](Request* req, Response* res) {
    res->SetStatus("404 Not Found");
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SendFile("www/404.html");
  });

  Server.Default([](Request* req, Response* res) {
    res->Error(404);
  });

  // Return Index Page
  Server.Get("/", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SendFile("www/index.html");
  });

  // Return stylesheet for index
  Server.Get("/css/index.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SendFile("www/css/index.css");
  });

  // Return javascript for index
  Server.Get("/js/index.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SendFile("www/js/index.js");
  });

  // Return font for logo
  Server.Get("/fonts/saldasoftheavy.otf", [](Request* req, Response* res) {
    res->SendFile("www/fonts/saldasoftheavy.otf");
  });

  std::cout << "Server running on port 3000" << std::endl;
  Server.Start();
  return 0;
}