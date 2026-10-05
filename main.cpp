#include <iostream>
#include "ladybug/LadyBugServer.hpp"

namespace lb = ladybug;

int main(int argc, char** argv){
    lb::LadyBugServer lbserver;
    lbserver.start();
}
