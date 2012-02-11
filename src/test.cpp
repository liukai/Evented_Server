#include "request.h"
#include <cassert>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <iostream>

using namespace std;

void test_parse_request() {
    Request request;

    {
        string req = "/";
        assert(parse_request(req.c_str(), req.size(), &request));
        
        assert(request.path == "/");
        assert(request.parameters.size() == 0);
    }

    {
        string req = "hello.cpp/anyway";
        assert(parse_request(req.c_str(), req.size(), &request));
        
        assert(request.path == req);
        assert(request.parameters.size() == 0);
    }

    {
        string req = "hello.cpp/anyway?";
        parse_request(req.c_str(), req.size(), &request);
        
        assert(request.path == "hello.cpp/anyway");
        assert(request.parameters.size() == 0);
    }
    {
        string req = "hello.cpp/anyway?a=b";
        parse_request(req.c_str(), req.size(), &request);
        
        assert(request.path == "hello.cpp/anyway");
        assert(request.parameters.size() == 1);
        assert(request.parameters["a"] == "b");
    }

    {
        string req = "hello.cpp/anyway?a=b&liu=kai&niu=mang";
        parse_request(req.c_str(), req.size(), &request);
        
        assert(request.path == "hello.cpp/anyway");
        assert(request.parameters.size() == 3);
        assert(request.parameters["a"] == "b");
        assert(request.parameters["liu"] == "kai");
        assert(request.parameters["niu"] == "mang");
    }
}

void test_fork() {
    int line[2];

    pipe(line);

    if(fork())
    {
        close(line[1]);
        // dup2(line[0], STDIN_FILENO);
        char buffer[1024];
        int bufferRead = 0;
        while ((bufferRead = read(line[0], buffer, 1024)) > 0) {
            cout<<"From client with love: "<<bufferRead<<" : "<<buffer<<endl;
        }
    }
    else {
        close(line[0]);
        dup2(line[1], STDOUT_FILENO);
        dup2(line[1], STDERR_FILENO);

        execlp( "cat", "cat", "Makefile", NULL );
    }
}

int main(int argc, char** argv) {
    // test_fork();
    test_parse_request();    
    return 0;
}
