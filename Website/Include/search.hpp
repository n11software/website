#ifndef search_hpp
#define search_hpp
#include <string>
#include <link>

int addSearchTerm(std::string term);
std::string getSuggestions(std::string term);
std::string getResults(std::string query, int page, std::string cookies, std::string lang, Request* req, Response* res);

#endif /* search_hpp */