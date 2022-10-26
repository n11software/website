#ifndef search_hpp
#define search_hpp
#include <string>

int addSearchTerm(std::string term);
std::string getSuggestions(std::string term);
std::string getResults(std::string query, int page);

#endif /* search_hpp */