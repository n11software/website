#include <search.hpp>
#include <db.hpp>
#include <theme.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <zstr.hpp>
#include <utils.hpp>
#include <chrono>

int addSearchTerm(std::string term) {
  try {
    sql::Statement* stmt = getConnection()->createStatement();
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
    sql::Statement* stmt = getConnection()->createStatement();
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

std::string getResults(std::string query, int page, std::string cookies) {
  std::string data = query;
  std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
  if (query.length() <= 256) addSearchTerm(data);
  data = "";
  std::ifstream file("www/search.html");
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  str = theme(str, cookies);
  str = replace(str, "[query]", query);
  try {
    std::string _q = query;
    _q = replace(_q, "_", "\\_");
    _q = replace(_q, "%", "\\%");

    auto start = std::chrono::high_resolution_clock::now();
    std::string srch = "match(content) against ('" + _q + "' IN NATURAL LANGUAGE MODE)";

    sql::Statement* stmt = getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT SQL_CALC_FOUND_ROWS * FROM `index` WHERE (language='en' OR language='en-US') AND ("+srch+" OR (domain LIKE '%"+_q+"%' OR path LIKE '%"+_q+"%')) order by ((domain LIKE '%"+_q+"%' OR path LIKE '%"+_q+"%') AND "+srch+") desc OFFSET " + std::to_string(page*10) + " ROWS FETCH NEXT 10 ROWS ONLY");
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
    stmt = getConnection()->createStatement();
    res = stmt->executeQuery("SELECT FOUND_ROWS()");

    res->next();
    std::string info = std::to_string(res->getInt(1)) + " results in ";

    if (res->getInt(1) > 0) {
      int pages = ((res->getInt(1)-1)/10)+1;
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
    auto stop = std::chrono::high_resolution_clock::now();
    std::string time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()/1000.0);
    info += time.substr(0, time.find_last_of('.')+3) + "s";
    str = replace(str, "[info]", info);
  } catch (sql::SQLException &e) {
    if (e.getErrorCode() == 2013) {
      connect();
      return getResults(query, page, cookies);
    }
    std::cout << "Error: " << e.what() << std::endl;
  }
  str = replace(str, "[results]", data);
  return str;
}