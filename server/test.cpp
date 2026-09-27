#include "json.hpp"
#include "OllamaClient.hpp"
#include "ModelConfig.hpp"
#include <iostream>
#include <string>

int main(){

    //EMPTY: only stream was populated and false lol; that may be a problem
    std::string model;
    std::string from;
    std::string system;
    Parameters param;


    // std::string model = "test4";
    // std::string from = "phi:latest"; 
    // std::string system = "answer in poop emojis";
    // Parameters param;
    // param.num_ctx = 4096;
    // param.num_predict = 1024;
    


    ModelConfig test4(model, from, system, param);

    nlohmann::json j = test4;
    
    // std::cout << j << std::endl;

    OllamaClient client;
    OllamaRes res;

    res = client.create(test4);

    std::cout << std::boolalpha;
    std::cout << res.response << "\nstatus: " << res.success << std::endl;


}