

.PHONY: clean build all

TARGET=client.e
OBJECTS=client.cpp

all: $(TARGET)

clean:
	rm -rf client.e $(OBJECTS)

$(TARGET): $(OBJECTS)
	g++ $^ -o $@ 

%.o: %.cpp %.h 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(BIN)/$@ $<

