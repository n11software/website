#include <link>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <zstr.hpp>
#include <chrono>
#include <utils.hpp>
#include <theme.hpp>
#include <db.hpp>
#include <search.hpp>
#include <api.hpp>

int main() {
  connect();
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
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/index.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return robots.txt
  Server.Get("/robots.txt", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/text; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/robots.txt");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return sitemap.xml
  Server.Get("/sitemap.xml", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "application/xml; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/sitemap.xml");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return stylesheet for index
  Server.Get("/css/index.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/index.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  Server.Get("/css/theme.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/theme.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return stylesheet for search
  Server.Get("/css/search.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/search.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return stylesheet for profile
  Server.Get("/css/profile.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/profile.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return javascript for searchbar
  Server.Get("/js/searchbar.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/searchbar.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return javascript for profile
  Server.Get("/js/profile.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/profile.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return javascript for theming
  Server.Get("/js/theme.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/theme.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return login
  Server.Get("/login", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    std::ifstream file("www/login.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string accs = "";
    sql::Statement* stmt = getConnection()->createStatement();
    std::vector<std::string> cookies = split(req->GetHeader("cookie"), "; ");
    std::string session = "";
    for (std::string cookie: cookies) {
      if (cookie[cookie.length()-1] == '\r') cookie = cookie.substr(0, cookie.length()-1);
      if (cookie.substr(0,8) == "session=") {
        session = cookie.substr(8);
        break;
      }
    }
    sql::ResultSet* rs = stmt->executeQuery("SELECT * FROM sessions WHERE id = '" + session + "'");
    if (!rs->next()) {
      str = replace(str, "[nothidden]", "hidden");
      str = replace(str, "[hidden]", "");
      res->SetHeader("Content-Encoding", "gzip");
      std::string compressed = compress(theme(str, req->GetHeader("cookie")));
      res->Send(compressed);
      return;
    }
    str = replace(str, "[nothidden]", "");
    str = replace(str, "[hidden]", "hidden");
    int i = 0;
    for (std::string token: split(rs->getString("tokens"), ";")) {
      rs = stmt->executeQuery("SELECT * FROM tokens WHERE id = '" + token + "'");
      if (!rs->next()) {
        // if token is invalid force relogin
        res->Error(403);
        return;
      }
      rs = stmt->executeQuery("SELECT * FROM accounts WHERE uuid = '" + rs->getString("uuid") + "'");
      if (!rs->next()) {
        // if account is invalid force relogin
        res->Error(403);
        return;
      }
      accs += "<div class=\"account\" onclick=\"LoginPreExisting(" + std::to_string(i) + ")\"> \
          <img src=\"/api/user/pfp?email=" + rs->getString("email") + "\"> \
          <div class=\"info\"> \
            <span class=\"name\">" + rs->getString("firstname") + " " + rs->getString("lastname") + "</span> \
            <span class=\"email\">" + rs->getString("email") + "</span> \
          </div> \
        </div>";
      i++;
    }
    str = replace(str, "[acclist]", accs);
    res->SetHeader("Content-Encoding", "gzip");
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return javascript for login
  Server.Get("/js/login.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/login.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return css for login
  Server.Get("/css/login.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/login.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  Server.Get("/api/user/exists", CheckUserExist);
  Server.Get("/api/user/pfp", GetUserPFP);
  Server.Post("/api/user/login", UserLogin);
  Server.Get("/api/user/login", UserLogin);

  // Search Page
  Server.Get("/search", [](Request* req, Response* res) {
    if (req->GetQuery("q") == "") {
      res->SetHeader("Location", "/");
      res->SetStatus("302 Found");
      res->Send("");
    } else {
      res->SetHeader("Content-Type", "text/html; charset=utf-8");
      res->SetHeader("Content-Encoding", "gzip");
      if (req->GetQuery("suggestions")=="true") {
        res->Send(getSuggestions(req->GetQuery("q")));
      } else {
        int page = 0;
        if (req->GetQuery("page") != "") {
          page = std::stoi(req->GetQuery("page"))-1;
        }
        std::string lang = "en";
        if (req->GetHeader("cookie").find("lang=") != std::string::npos) {
          lang = req->GetHeader("cookie").substr(req->GetHeader("cookie").find("lang=")+5, 2);
        }
        std::string data = getResults(req->GetQuery("q"), page, req->GetHeader("cookie"), lang);
        std::string compressed = compress(data);
        res->Send(compressed);
      }
    }
  });

  // Fonts
  Server.Get("/fonts/jbm.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/fonts/jbm.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  Server.Get("/fonts/jbm.woff2", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "font/woff2");
    res->SetHeader("Cache-Control", "public, max-age=31536000");
    res->SetHeader("Accept-Ranges", "bytes");
    res->SendFile("www/fonts/jbm.woff2");
  });

  // Favicon
  Server.Get("/android-chrome-192x192.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/android-chrome-192x192.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/android-chrome-512x512.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/android-chrome-512x512.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/apple-touch-icon.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/apple-touch-icon.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/browserconfig.xml", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "application/xml; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/browserconfig.xml");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/favicon-16x16.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/favicon-16x16.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/favicon-32x32.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/favicon-32x32.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/favicon.ico", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/x-icon; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/favicon.ico");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/mstile-150x150.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/mstile-150x150.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/safari-pinned-tab.png", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "image/png; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/safari-pinned-tab.png");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/site.webmanifest", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "application/manifest+json; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    res->SetHeader("Cache-Control", "private, max-age=86400, stale-while-revalidate=604800");
    std::ifstream file("www/favicon/site.webmanifest");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  std::cout << "Server running on port 3000" << std::endl;
  Server.Start();
  return 0;
}