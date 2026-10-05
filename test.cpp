#include "json.hpp"
#include "ladybug/OllamaClient.hpp"
#include "ladybug/ModelConfig.hpp"
#include <iostream>
#include <string>

using namespace ladybug;

int main(){

    //EMPTY: only stream was populated and false lol; that may be a problem
    // std::string model;
    // std::string from;
    // std::string system;
    // Parameters param;


    // std::string model = "test4";
    // std::string from = "phi:latest"; 
    // std::string system = "You are an assistant. You answer in as brief words as possible. As accurate as possible.";
    // Parameters param;
    // param.num_ctx = 4096;
    // param.num_predict = 1024;

    std::string model = "test-lb";
    std::string from = "qwen3:4b "; 
    std::string system = "You are an assistant. You answer as accurate as possible and succinctly.";
    Parameters param;
    param.num_ctx = 8192;
    param.num_predict = 1024;
    


    ModelConfig test4(model, from, system, param);

    nlohmann::json j = test4;
    
    // std::cout << j << std::endl;

    OllamaClient client;
    OllamaRes res;

    res = client.create(test4);

    std::cout << std::boolalpha;
    std::cout << res.response << "\nstatus: " << res.success << std::endl;



    std::cout << "\n\n\n";

    //////////////////////////////

    // EMPTY
    // std::string model;
    // std::string system;
    // Options param;


    GenRequest req(model);
    req.setPrompt("what is the largest state by area, by population and by national park area?");
    OllamaRes resGen = client.generate(req);
    std::cout << resGen.response << "\nstatus: " << resGen.success << std::endl;


}