CXX = g++
CXXFLAGS = -pthread -std=c++11
LDFLAGS = -pthread
SOURCE = $(ptullmann.cpp)
OBJECTS = $(SOURCE:.cpp=.o)
TARGET = ptullmann

default: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

ptullmann: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET)