#include "request.h"
#include <algorithm>
#include <cassert>

using namespace std;

bool parse_request(const char* request, int size, Request* pReq) {
    // find the first '?' to separate the path and the parameters
    const char* pos = find(request, request + size, '?'); 
    pReq->path.assign(request, pos);

    if (pos == request + size) {
        return true;
    }

    assert(*pos == '?');
    // skip the '?'
    pos += 1;

    while (pos != request + size) {
        const char* end = find(pos, request + size, '&');
        // if the parameter is not empty
        // TODO: trimming of the url
        // TODO: what if there are duplicated parameter?
        if (pos != request + size) {
            const char* sep = find(pos, end, '=');
            // check if there is empty key or empty value
            if (pos == sep || sep == end || sep + 1 == end) {
                return false;
            }
            pair<string, string> param = make_pair(string(pos, sep), string(sep + 1, end));
            pReq->parameters.insert(param);
        }
        if (*end == '&')
            pos = end + 1;
        else
            pos = end;
    }
    return true;
}

bool parse_url(const char* request, int size, boost::filesystem::path* pPath) {
    const char* GET_REQUEST = "GET ";

    // TODO: lower case also counts
    // should also trim the string first
    const char* NONE = 0;
    if (!equal(GET_REQUEST, GET_REQUEST + strlen(GET_REQUEST), request)) 
        return false;

    request += strlen(GET_REQUEST);
    const char* begin = request;
    while(*request != ' ') {
        ++request;
    }

    const char* end = request;

    const char* PROTOCOL = "HTTP";
    // and check the size
    if (!equal(PROTOCOL, PROTOCOL + strlen(PROTOCOL), request + 1)) {
        return false;
    }

    pPath->assign(begin, end);
    return true;
}
