#ifndef IP_SO_ASPECT
#define IP_SO_ASPECT

#include "client_proxy.h"
#include <string>
#include <map>

typedef bool (*Writer)(int, const char*);
class SoClientProxy : public ClientProxy {
public:
    SoClientProxy(int port): ClientProxy(port) {}
    virtual const char* get_directory() const { return "cgi-bin"; }
protected:
    virtual bool load_dynamic_content(const char*  resource, const char* query);
private:
    static std::map<std::string, Writer> libs;
};

#endif
