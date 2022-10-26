#include <link>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <zstr.hpp>
#include "mysql_driver.h" 
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <chrono>

sql::Driver* driver;
sql::Connection* db;

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
    if (token != "") result.push_back(token);
    data.erase(0, pos + delimiter.length());
  }
  if (data != "") result.push_back(data);
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

void connect() {
  std::string db_user, db_pass, db_host, line;
  std::ifstream db_file("db.txt");
  while (std::getline(db_file, line)) {
    if (line.find("user=") != std::string::npos) {
      db_user = line.substr(line.find("user=")+5);
    } else if (line.find("pass=") != std::string::npos) {
      db_pass = line.substr(line.find("pass=")+5);
    } else if (line.find("host=") != std::string::npos) {
      db_host = line.substr(line.find("host=")+5);
    }
  }
  try {
    driver = get_driver_instance();
    db = driver->connect("tcp://" + db_host + ":3306", db_user, db_pass);
    db->setSchema("n11");
  } catch (sql::SQLException &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

int addSearchTerm(std::string term) {
  try {
    sql::Statement* stmt = db->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM query WHERE query = '" + term + "'");
    if (res->next()) {
      stmt->execute("UPDATE query SET searches = searches + 1 WHERE query = '" + term + "'");
    } else {
      stmt->execute("INSERT INTO query (query, searches) VALUES ('" + term + "', 1)");
    }
    delete res;
    delete stmt;
    return 0;
  } catch (sql::SQLException &e) {
    if (e.getErrorCode() == 2013) {
      connect();
      return addSearchTerm(term);
    }
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }
}

std::string getSuggestions(std::string term) {
  std::string data = "{\"suggestions\":[";
  try {
    term = replace(term, "_", "\\o");
    std::vector<std::string> words = split(term, " ");
    std::string q = "SELECT * FROM query WHERE";
    int x = 0;
    for (std::string word: words) {
      if (x == 0) {
        if (word[0] == '-') {
          q += " query NOT LIKE '%" + word.substr(1) + "%'";
        } else {
          q += " query LIKE '%" + word + "%'";
        }
      } else {
        if (word[0] == '-') {
          q += " AND query NOT LIKE '%" + word.substr(1) + "%'";
        } else {
          q += " AND query LIKE '%" + word + "%'";
        }
      }
      x++;
    }
    if (x == 0) {
      return compress("{\"error\":\"Could not parse terms!\"}");
    }
    sql::Statement* stmt = db->createStatement();
    sql::ResultSet* res = stmt->executeQuery(q+" ORDER BY searches DESC LIMIT 5");
    int i = 0;
    while (res->next()) {
      data += "\"" + res->getString("query") + "\",";
      i++;
    }
    data = data.substr(0, data.length()-1);
    data += "]}";
    if (i == 0) {
      data = "{\"suggestions\":[]}";
    }
  } catch (sql::SQLException &e) {
    if (e.getErrorCode() == 2013) {
      connect();
      return compress(getSuggestions(term));
    }
    std::cout << "Error: " << e.what() << std::endl;
  }
  return compress(data);
}

std::string getResults(std::string query, int page) {
  std::string data = query;
  std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
  if (query.length() <= 256) addSearchTerm(data);
  data = "";
  std::ifstream file("www/search.html");
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  str = replace(str, "[query]", query);
  try {
    std::string _q = query;
    _q = replace(_q, "_", "\\_");
    _q = replace(_q, "%", "\\%");
    std::vector<std::string> words = split(_q, " ");
    std::string q = "SELECT * FROM `index` WHERE (language='en' OR language='en-US') AND (";
    if (words.size() == 0) {
      return "{\"error\":\"Could not parse terms!\"}";
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::string srch = "match(content) against ('" + _q + "' IN NATURAL LANGUAGE MODE)";
    sql::Statement* stmt = db->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM `index` WHERE (language='en' OR language='en-US') AND ("+srch+" OR (domain LIKE '%"+_q+"%' OR path LIKE '%"+_q+"%'))");
    std::string info = std::to_string(res->rowsCount()) + " results in ";

    if (res->rowsCount() > 0) {
      int pages = ((res->rowsCount()-1)/10)+1;
      std::string pagesData = (page>0? "<a href=\"/search?q=" + _q + "&page=" + std::to_string(page) + "\" class=\"page\">Previous</a>": "");
      if (page <= 4) {
        int max = 10;
        if (pages < 10) max = pages;
        for (int i = 1;i<=max;i++) {
          if (page+1 == i) pagesData += "<span class=\"page\">" + std::to_string(i) + "</span>";
          else pagesData += "<a href=\"/search?q=" + _q + "&page=" + std::to_string(i) + "\" class=\"page\">" + std::to_string(i) + "</a>";
        }
      } else {
        for (int i = page-4;i<=page+5;i++) {
          if (i > pages) break;
          if (page+1 == i) pagesData += "<span class=\"page\">" + std::to_string(i) + "</span>";
          else pagesData += "<a href=\"/search?q=" + _q + "&page=" + std::to_string(i) + "\" class=\"page\">" + std::to_string(i) + "</a>";
        }
      }
      if (page+1<pages) pagesData += "<a href=\"/search?q=" + _q + "&page=" + std::to_string(page+2) + "\" class=\"page\">Next</a>";
      str = replace(str, "[pages]", pagesData);
    } else {
      str = replace(str, "[pages]", "");
    }

    stmt = db->createStatement();
    res = stmt->executeQuery("SELECT * FROM `index` WHERE (language='en' OR language='en-US') AND ("+srch+" OR (domain LIKE '%"+_q+"%' OR path LIKE '%"+_q+"%')) order by ((domain LIKE '%"+_q+"%' OR path LIKE '%"+_q+"%') AND "+srch+") desc OFFSET " + std::to_string(page*10) + " ROWS FETCH NEXT 10 ROWS ONLY");
    int i = 0;
    while (res->next()) {
      std::string url = res->getString("protocol") + "://" + res->getString("domain");
      std::string path;
      if (res->getString("path").substr(0, 1) != "/") path = "/" + res->getString("path");
      else path = res->getString("path");
      url += path;
      std::vector<std::string> files = split(path, "/");
      std::string cite = "";
      for (std::string file: files) {
        if (cite == "") cite = "<span class=\"path\">";
        if (file.find_first_of("?") != std::string::npos) {
          std::vector<std::string> _file = split(file, "?");
          if (_file.size() == 2) {
            file = _file[0];
            if (file.find_last_of(".") != std::string::npos) {
              cite += " › " + file.substr(0, file.find_last_of("."));
            } else cite += " › " + file;
          } else {
            cite += " › " + file;
          }
        } else {
          if (file.find_last_of(".") != std::string::npos) {
            cite += " › " + file.substr(0, file.find_last_of("."));
          } else cite += " › " + file;
        }
        if (cite.length() > 47) {
          cite = cite.substr(0, 47) + "...";
          break;
        }
      }
      if (cite != "") cite += "</span>";
      data += "<div class=\"result\">";
      std::string title = res->getString("title");
      if (replace(replace(res->getString("title"), " ", ""), "\n", "").length() <= 0) title = url;
      data += "<a href=\"" + url + "\"><cite class=\"url\" role=\"text\">" + res->getString("protocol") + "://" + res->getString("domain") + cite + "</cite><span class=\"title\">" + title + "</span></a>\n";
      data += "<span class=\"description\">" + res->getString("description") + "</span>\n";
      data += "</div>\n";
      i++;
    }
    if (i == 0) {
      data = "<span class=\"nrf\">No results found!</span>";
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::string time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()/1000.0);
    info += time.substr(0, time.find_last_of('.')+3) + "s";
    str = replace(str, "[info]", info);
  } catch (sql::SQLException &e) {
    if (e.getErrorCode() == 2013) {
      connect();
      return getResults(query, page);
    }
    std::cout << "Error: " << e.what() << std::endl;
  }
  str = replace(str, "[results]", data);
  return str;
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
        std::string data = getResults(req->GetQuery("q"), page);
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