include ../common.mk

EVA_OBJECTS=eva.cpp $(BIN)/request.o $(BIN)/content_loader.o \
			$(BIN)/util.o $(BIN)/server.o \
			$(BIN)/config.o $(BIN)/client_proxy.o $(BIN)/log.o \
			$(BIN)/cgi_client_proxy.o $(BIN)/so_client_proxy.o

.PHONY: all clean
all: $(BIN)/eva.e
clean:
	rm -rf $(BIN)/*.e $(BIN)/*.o $(BIN)/eva.e.dSYM

$(BIN)/eva.e: $(EVA_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ 
