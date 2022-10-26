#include <theme.hpp>
#include <utils.hpp>
#include <iostream>
#include <fstream>
#include <vector>

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