#include <link>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <zstr.hpp>
#include <chrono>
#include <utils.hpp>
#include <db.hpp>
#include <search.hpp>
#include <api.hpp>

void search(Request* req, Response* res) {
  if (req->GetQuery("q") == "") {
    if (req->GetQuery("u") != "") res->SetHeader("Location", "/u/"+req->GetQuery("u"));
    else res->SetHeader("Location", "/");
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
      std::string data = getResults(req->GetQuery("q"), page, req->GetHeader("cookie"), lang, req, res);
      std::string compressed = compress(data);
      res->Send(compressed);
    }
  }
}

void redir(Response* res, std::string url) {
  res->SetHeader("Location", url);
  res->SetStatus("302 Found");
  res->Send("");
}

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
    std::string uuid = GetUserID(getCookie(req->GetHeader("cookie"), "session"), "0");
    if (uuid != "") redir(res, "/u/0");
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/index.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    str = replace(str, "[userinfo]", "<a href=\"/login\" class=\"login\">Login</a>");
    str = replace(str, "[user]", "-1");
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Accounts page
  Server.Get("/u/{user}/account", [](Request* req, Response* res) {
    res->Send("Account page");
  });

  // Return index with user
  Server.Get("/u/{user}", [](Request* req, Response* res) {
    if (!isINT(req->GetQuery("user"))) redir(res, "/");
    std::string uuid = GetUserID(getCookie(req->GetHeader("cookie"), "session"), req->GetQuery("user"));
    if (uuid == "") redir(res, "/");
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    UserInfo u(uuid);
    std::ifstream file("www/index.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    str = AddUserInfo(str, uuid, req->GetQuery("user"), req->GetHeader("cookie"), "/u/{user}");
    str = replace(str, "[user]", req->GetQuery("user"));
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Account
  Server.Get("/u/{user}/account", [](Request* req, Response* res) {
    if (!isINT(req->GetQuery("user"))) redir(res, "/");
    std::string uuid = GetUserID(getCookie(req->GetHeader("cookie"), "session"), req->GetQuery("user"));
    if (uuid == "") redir(res, "/");
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    UserInfo u(uuid);
    std::ifstream file("www/account.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    str = AddUserInfo(str, uuid, req->GetQuery("user"), req->GetHeader("cookie"), "/u/{user}/account");
    str = replace(str, "[user]", req->GetQuery("user"));
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  Server.Get("/u/{user}/account/security", [](Request* req, Response* res) {
    if (!isINT(req->GetQuery("user"))) redir(res, "/");
    std::string uuid = GetUserID(getCookie(req->GetHeader("cookie"), "session"), req->GetQuery("user"));
    if (uuid == "") redir(res, "/");
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    UserInfo u(uuid);
    std::ifstream file("www/security.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    str = AddUserInfo(str, uuid, req->GetQuery("user"), req->GetHeader("cookie"), "/u/{user}/account");
    str = replace(str, "[user]", req->GetQuery("user"));
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  Server.Get("/css/account.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/account.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
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
    std::string compressed = compress(str);
    res->Send(compressed);
  });
  Server.Get("/css/profile.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/profile.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return stylesheet for search
  Server.Get("/css/search.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/search.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return javascript for searchbar
  Server.Get("/js/searchbar.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/searchbar.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
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
    if (!rs->next() || req->GetQuery("skipselect") == "true") {
      str = replace(str, "[nothidden]", "hidden");
      str = replace(str, "[hidden]", "");
      res->SetHeader("Content-Encoding", "gzip");
      std::string compressed = compress(str);
      res->Send(compressed);
      return;
    }
    str = replace(str, "[nothidden]", "");
    str = replace(str, "[hidden]", "hidden");
    int i = 0;
    for (std::string token: split(rs->getString("tokens"), ";")) {
      rs = stmt->executeQuery("SELECT * FROM tokens WHERE id = '" + token + "'");
      if (!rs->next()) {
        // TODO: if token is invalid force relogin
        res->Error(403);
        res->Send(compress("You have been logged out!"));
        return;
      }
      rs = stmt->executeQuery("SELECT * FROM accounts WHERE uuid = '" + rs->getString("uuid") + "'");
      if (!rs->next()) {
        // TODO: if account is invalid force relogin
        res->Error(403);
        res->Send(compress("One of the accounts was deleted!"));
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
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return css for login
  Server.Get("/css/login.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/login.css");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return signup
  Server.Get("/signup", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/html; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/signup.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return javascript for signup
  Server.Get("/js/signup.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/signup.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  // Return javascript for profile
  Server.Get("/js/profile.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/profile.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(str);
    res->Send(compressed);
  });

  Server.Get("/api/user/exists", CheckUserExist);
  Server.Get("/api/user/pfp", GetUserPFP);
  Server.Post("/api/user/login", UserLogin);
  Server.Post("/api/user/create", UserCreate);

  // Search Page
  Server.Get("/search", search);

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