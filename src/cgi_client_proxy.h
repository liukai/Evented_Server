#ifndef IP_CGI_ASPECT
#define IP_CGI_ASPECT

#include "client_proxy.h"

class CGIClientProxy : public ClientProxy {
public:
    CGIClientProxy(int port): ClientProxy(port) {}
    virtual const char* get_directory() const { return "cgi"; }
protected:
    virtual bool load_dynamic_content(const char*  resource, const char* query);
};

#endif
