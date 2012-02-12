.PHONY: clean cgi so

BIN=../bin
SO=../cgi-bin
CGI=../cgi
CPPFLAGS=-g -Wall
CXXFLAGS=-levent
CXXFLAGS+=-lboost_filesystem -lboost_system

$(BIN)/%.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) -c -o $@ $<

