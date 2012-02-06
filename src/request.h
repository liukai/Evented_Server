#ifndef IP_REQUEST
#define IP_REQUEST

#include <string>
#include <map>
#include <boost/filesystem.hpp>

typedef std::pair<const char*, const char*> range;
struct Request {
    std::string path;
    std::map<std::string, std::string> parameters;
};

bool parse_request(const char* request, int size, Request* pReq);
bool parse_url(const char* request, int size, boost::filesystem::path* pPath);

#endif
