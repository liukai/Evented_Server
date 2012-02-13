#ifndef IP_CONFIG_H
#define IP_CONFIG_H

#include <set>
#include <map>
#include <string>

enum ResourceType {CGI, STATIC, INVALID};
class Config {
public:
    Config(const char* filename);
    ResourceType get_file_type(const char* resource_begin,
                               const char* resource_end,
                               std::string& type) const;
    const char* get_result_type(const char* type) const;
    static const char* DEFAULT_TYPE;
private:
    void load_files(std::set<std::string>& file_set, 
                    const std::string& raw_text);
    std::set<std::string> static_files;
    std::map<std::string, std::string> return_types;
    std::set<std::string> cgi_files;
};

#endif
