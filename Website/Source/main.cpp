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
    _q = replace(_q, "_", "\\o");
    std::vector<std::string> words = split(_q, " ");
    std::string q = "SELECT * FROM `index` WHERE (language='en' OR language='en-US') AND (";
    if (words.size() == 0) {
      return "{\"error\":\"Could not parse terms!\"}";
    }
    sql::Statement* stmt = db->createStatement();
    sql::ResultSet* res = stmt->executeQuery(q+"title LIKE '%" + query + "%' OR description LIKE '%" + query + "%' OR keywords LIKE '%" + query + "%') OFFSET " + std::to_string(page*10) + " ROWS FETCH NEXT 10 ROWS ONLY");
    int i = 0;
    while (res->next()) {
      data += "<a href=\"" + res->getString("protocol") + "://" + res->getString("domain") + res->getString("path") + "\">" + res->getString("title") + "</a>\n";
      data += "<span>" + res->getString("description") + "</span>\n";
      data += "<span>" + res->getString("language") + "</span>\n";
      i++;
    }
    if (i == 0) {
      data = "No results found!";
    }
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

  // Return stylesheet for index
  Server.Get("/css/index.css", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/css; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/css/index.css");
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

  // Return javascript for index
  Server.Get("/js/index.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/index.js");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string compressed = compress(theme(str, req->GetHeader("cookie")));
    res->Send(compressed);
  });

  // Return javascript for index
  Server.Get("/js/search.js", [](Request* req, Response* res) {
    res->SetHeader("Content-Type", "text/javascript; charset=utf-8");
    res->SetHeader("Content-Encoding", "gzip");
    std::ifstream file("www/js/search.js");
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
          page = std::stoi(req->GetQuery("page"));
        }
        std::string compressed = compress(getResults(req->GetQuery("q"), page));
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