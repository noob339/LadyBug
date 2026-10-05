CC  = g++ -pthread
CXX = g++ -pthread

INCLUDES = -Iinclude -Ithird-party
CFLAGS   = -std=c++17 -g -Wall $(INCLUDES)
CXXFLAGS = -std=c++17 -g -Wall $(INCLUDES)

default: main test
main: main.o src/LadyBugServer.o src/OllamaClient.o src/ModelConfig.o src/GenRequest.o

test: test.o src/OllamaClient.o src/ModelConfig.o src/GenRequest.o

# header dependency
# header dependencies
main.o: main.cpp third-party/httplib.h third-party/json.hpp include/ladybug/CrossPlatform.hpp include/ladybug/OllamaClient.hpp include/ladybug/ModelConfig.hpp include/ladybug/LadyBugServer.hpp include/ladybug/GenRequest.hpp

test.o: test.cpp third-party/httplib.h third-party/json.hpp include/ladybug/OllamaClient.hpp include/ladybug/ModelConfig.hpp


run: main
	./main

.PHONY: clean
clean:
	rm -f *.o *~ a.out core main test


