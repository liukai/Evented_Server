#ifndef IP_CONFIG_H
#define IP_CONFIG_H

#include <set>
#include <string>

enum ResourceType {CGI, STATIC, INVALID};
class Config {
public:
    Config(const char* filename);
    ResourceType get_file_type(const char* resource_begin,
                               const char* resource_end) const;
private:
    void load_files(std::set<std::string>& file_set, 
                    const std::string& raw_text);
    std::set<std::string> static_files;
    std::set<std::string> cgi_files;
     
};

#endif
