#include <link>
#include <iostream>
#include "../Include/zstr.hpp"

std::string compress(std::string data) {
  std::ostringstream output;
  std::istringstream ds(data);

  zstr::ostream zs(output);
  const std::streamsize size = 1 << 16;
  char* buf = new char[size];
  while (true) {
    ds.read(buf, size);
    std::streamsize count = ds.gcount();
    if (count == 0) break;
    zs.write(buf, count);
  }
  delete [] buf;
  return (std::ostringstream&)zs << std::flush, output.str();
}

std::vector<std::string> split(std::string data, std::string delimiter) {
  std::vector<std::string> result;
  size_t pos = 0;
  std::string token;
  while ((pos = data.find(delimiter)) != std::string::npos) {
    token = data.substr(0, pos);
    result.push_back(token);
    data.erase(0, pos + delimiter.length());
  }
  result.push_back(data);
  return result;
}

std::string replace(std::string data, std::string delimiter, std::string replacement) {
  size_t pos = 0;
  while ((pos = data.find(delimiter)) != std::string::npos) {
    data.replace(pos, delimiter.length(), replacement);
  }
  return data;
}

std::string theme(std::string data, std::string cookies) {
  if (cookies.find("theme=") != std::string::npos) {
    std::vector<std::string> theme = split(cookies.substr(cookies.find("theme=")+6), ",");
    std::string var;
    for (int i = 0; i < theme.size(); i++) {
      switch (i) {
        case 0:
          data = replace(data, "[background]", theme[i]);
          break;
        case 1:
          data = replace(data, "[content]", theme[i]);
          break;
        case 2:
          data = replace(data, "[placeholder]", theme[i]);
          break;
        case 3:
          data = replace(data, "[text]", theme[i]);
          break;
        case 4:
          data = replace(data, "[subtext]", theme[i]);
          break;
        case 5:
          data = replace(data, "[btn]", theme[i]);
          break;
        case 6:
          data = replace(data, "[btntxt]", theme[i]);
          break;
        case 7:
          data = replace(data, "[subbtn]", theme[i]);
          break;
        case 8:
          data = replace(data, "[subbtntxt]", theme[i]);
          break;
        case 9:
          if (theme[i].substr(0, theme[i].length()-1) == "true") {
            data = replace(data, "/* [shadow] ", "");
            data = replace(data, " */", "");
            data = replace(data, "{shadows}", "true");
            data = replace(data, "[shch]", "checked");
          } else {
            data = replace(data, "{shadows}", "false");
            data = replace(data, "[shch]", "");
          }
          break;
      }
    }
  } else {
    data = replace(data, "[background]", "#f8f8f8");
    data = replace(data, "[content]", "#ffffff");
    data = replace(data, "[placeholder]", "#a9a9a9");
    data = replace(data, "[text]", "#1c1917");
    data = replace(data, "[subtext]", "#5e5e5e");
    data = replace(data, "[btn]", "#000000");
    data = replace(data, "[btntxt]", "#ffffff");
    data = replace(data, "[subbtn]", "#eeeeee");
    data = replace(data, "[subbtntxt]", "#5e5e5e");
    data = replace(data, "/* [shadow] ", "");
    data = replace(data, " */", "");
    data = replace(data, "{shadows}", "true");
    data = replace(data, "[shch]", "checked");
  }
  return data;
}

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
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/index.html");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
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

  // Return javascript for index
  Server.Get("/js/index.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/index.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Search Page
  Server.Get("/search", [](Request* req, Response* res) {
    if (req->GetQuery("q") == "") {
      res->Error(404);
    } else {
      res->SetHeader("Content-Type", "text/html; charset=utf-8");
      res->SetHeader("Content-Encoding", "gzip");
      if (req->GetQuery("suggestions")=="true") {
        std::string compressed = compress("{\"suggestions\":[ \"Hi\", \"Hello\", \"Bye\" ]}");
        res->Send(compressed);
      } else {
        std::ifstream file("www/search.html");
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string compressed = compress(str);
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

  std::cout << "Server running on port 3000" << std::endl;
  Server.Start();
  return 0;
}