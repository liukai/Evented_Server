.PHONY: clean build all

CLIENT_OBJECTS=client.cpp
SERVER_OBJECTS=server.cpp $(BIN)/gateway.o
NONBLOCK_OBJECTS=nonblock.cpp
CXXFLAGS=-levent
BIN=bin

all: $(BIN)/client.e $(BIN)/server.e $(BIN)/nonblock.e

clean:
	rm -rf $(BIN)/*.e $(BIN)/*.o

$(BIN)/nonblock.e: $(NONBLOCK_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 
$(BIN)/server.e: $(SERVER_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 

$(BIN)/client.e: $(CLIENT_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 

$(BIN)/%.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

