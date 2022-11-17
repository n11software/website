#ifndef utils
#define utils
#include <string>
#include <vector>

std::string compress(std::string data);
std::vector<std::string> split(std::string data, std::string delimiter);
std::string replace(std::string data, std::string delimiter, std::string replacement);
std::string getCookie(std::string cookies, std::string name);
bool isINT(const std::string& s);

#endif /* utils */