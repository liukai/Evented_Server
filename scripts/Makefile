include ../common.mk

.PHONY: all clean
all: $(SO)/hello.shtml $(CGI)/hello.shtml
clean:
	rm -rf $(SO)/*.shtml $(CGI)/*.shtml

#-- CGI Scripts --
$(SO)/hello.shtml: hello.cpp
	g++ -fPIC -ldl -shared hello.cpp -o $(SO)/hello.shtml
$(CGI)/hello.shtml: cgi_hello.cpp
	g++ cgi_hello.cpp -o $(CGI)/hello.shtml
