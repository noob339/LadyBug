CC  = g++ -pthread
CXX = g++ -pthread

INCLUDES = 
CFLAGS   = -std=c++17 -g -Wall $(INCLUDES)
CXXFLAGS = -std=c++17 -g -Wall $(INCLUDES)
default: main test
main: main.o 
test: test.o
# header dependency
main.o: main.cpp httplib.h json.hpp CrossPlatform.hpp OllamaClient.hpp ModelConfig.hpp
test.o: main.cpp httplib.h json.hpp OllamaClient.hpp ModelConfig.hpp

run: main
	./main

.PHONY: clean
clean:
	rm -f *.o *~ a.out core main test


