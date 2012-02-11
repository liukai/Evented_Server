.PHONY: clean build all test

BIN=bin
# SERVER_OBJECTS=server.cpp $(BIN)/gateway.o 
EVA_OBJECTS=eva.cpp $(BIN)/request.o $(BIN)/content_loader.o \
			$(BIN)/util.o $(BIN)/server.o \
			$(BIN)/config.o $(BIN)/client_proxy.o $(BIN)/log.o
TEST_OBJECTS=test.cpp $(BIN)/request.o
ZMQ_OBJECTS=zmq.cpp
CPPFLAGS=-g
CXXFLAGS=-levent
CXXFLAGS+=-lboost_filesystem -lboost_system
CXXFLAGS+=-lzmq -llog4cxx

all: $(BIN)/eva.e $(BIN)/test.e $(BIN)/zmq.e

clean:
	rm -rf $(BIN)/*.e $(BIN)/*.o
test:
	make $(BIN)/test.e
	$(BIN)/test.e
run:
	make $(BIN)/eva.e
	$(BIN)/eva.e

$(BIN)/test.e: $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 
$(BIN)/eva.e: $(EVA_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 
$(BIN)/zmq.e: $(ZMQ_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 

$(BIN)/%.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) -c -o $@ $<

