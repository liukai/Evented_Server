#include "config.h"
#include <boost/property_tree/ini_parser.hpp>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace boost::property_tree;
using namespace std;

Config::Config(const char* filename) {
    ptree tree;
    ini_parser::read_ini(filename, tree);

    string static_files_text = tree.get<std::string>("resource.static");
    load_files(static_files, static_files_text);
    string dynamic_files_text = tree.get<std::string>("resource.dynamic");
    load_files(dynamic_files, dynamic_files_text);
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
                                   const char* resource_end) {
    typedef reverse_iterator<const char*> RIter;
    const char* revPos = find(RIter(resource_end), 
                                    RIter(resource_begin),
                                    '.').base();
    cout<<"extension: "<<revPos<<endl;
    if (revPos == resource_begin) {
        return INVALID;
    }
    string type(revPos, resource_end);
    return static_files.find(type) != static_files.end() ? STATIC :
           dynamic_files.find(type) != dynamic_files.end() ? DYNAMIC:
           INVALID;
}
