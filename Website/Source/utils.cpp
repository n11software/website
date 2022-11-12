#include <utils.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <zstr.hpp>

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

std::string getCookie(std::string data, std::string name) {
  std::vector<std::string> cookies = split(data, "; ");
  std::string c = "";
  for (std::string cookie: cookies) {
    if (cookie[cookie.length()-1] == '\r') cookie = cookie.substr(0, cookie.length()-1);
    if (cookie.substr(0,name.length()+1) == name+"=") {
      c = cookie.substr(name.length()+1);
      break;
    }
  }
  return c;
}