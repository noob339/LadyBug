#include <iostream>
#include "ModelConfig.hpp"
#include "OllamaClient.hpp"
#include "LadyBugServer.hpp"

int main(int argc, char** argv)
{
    LadyBugServer lbserver;
    lbserver.start();
}
