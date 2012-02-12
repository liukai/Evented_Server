#include <iostream>
using namespace std;

int main(void) {
    char hello[] = "<h1>hello world!</h1>";
    const char header[] = "HTTP/1.0 200 OK\r\n"
                          "CONTENT-TYPE: text/html\r\n\r\n";
    cout<<header<<endl;
    for (int i = 0; i < 1000; ++i)
        cout<<hello<<endl;
    
    /*char *buf = getenv("QUERY_STRING");
    cout<<buf<<endl;*/

    return 0;
}
