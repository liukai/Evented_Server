#include "config.h"
#include <boost/property_tree/ini_parser.hpp>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace boost::property_tree;
using namespace std;

const char* Config::DEFAULT_TYPE = "text/plain";

Config::Config(const char* filename) {
    ptree tree;
    ini_parser::read_ini(filename, tree);

    string static_files_text = tree.get<std::string>("static.types");
    load_files(static_files, static_files_text);
    string dynamic_files_text = tree.get<std::string>("cgi.types");
    load_files(cgi_files, dynamic_files_text);

    for (set<string>::const_iterator pos = static_files.begin();
         pos != static_files.end(); ++pos) {
        const string& type = *pos;
        string response_type = tree.get<std::string>("static." + type);
        return_types[type] = response_type;
    }
}

void Config::load_files(set<std::string>& file_set,
                        const string& raw_text) {
    string::const_iterator begin = raw_text.begin();
    string::const_iterator end = raw_text.end();

    do {
        string::const_iterator sep = find(begin, end, ' ');
        string s(begin, sep);
        file_set.insert(s);
        begin = sep + 1;
    } while(begin < end);
}

ResourceType Config::get_file_type(const char* resource_begin,
                                   const char* resource_end, string& type) const {
    typedef reverse_iterator<const char*> RIter;
    const char* revPos = find(RIter(resource_end), 
                                    RIter(resource_begin),
                                    '.').base();
    cout<<"extension: "<<revPos<<endl;
    if (revPos == resource_begin) {
        return INVALID;
    }
    type.assign(revPos, resource_end);
    return static_files.find(type) != static_files.end() ? STATIC :
           cgi_files.find(type) != cgi_files.end() ? CGI:
           INVALID;
}
const char* Config::get_result_type(const char* type) const {
    map<string, string>::const_iterator pos = return_types.find(type);
    if (pos == return_types.end())
        return DEFAULT_TYPE;
    return pos->second.c_str();
}
