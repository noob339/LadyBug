#ifndef OLLAMACLIENT_HPP
#define OLLAMACLIENT_HPP

#include <string>
#include <chrono>
#include <iostream>
#include "json.hpp"
#include "httplib.h"
#include "ladybug/GenRequest.hpp"
#include "ladybug/ModelConfig.hpp"

namespace ladybug{

// status code
// validate the incoming response from ollama api
// error handling
// timeouts, how to handle that

struct OllamaRes {

    std::string response;
    bool success;
    int status;

    OllamaRes(){
        this->response = "";
        this->success = false;
        this->status = 0;
    }

    OllamaRes(std::string response, int status){
        this->response = response;
        this->success = false; //many conditions where it could be false, but only one where it can be true
        this->status = status;
    }

    OllamaRes(std::string response, bool success, int status){
        this->response = response;
        this->success = success; //many conditions where it could be false, but only one where it can be true
        this->status = status;
    }

}; 


class OllamaClient{
    //what should the program do if parsing fails? just throw, log the error and return, it failed to parse for whatever reason, malformed json, etc

    private:
        const std::string baseAdd {"http://localhost:11434"};
        httplib::Client client {baseAdd};
        nlohmann::json defaultJson;
        std::chrono::seconds connectionTimeout{2}; 
        std::chrono::seconds writeTimeout{10};
        std::chrono::seconds readTimeout{90};


        // uses no data members so func belongs to class, not individual instance obj
        static std::string statusError (int code){

            std::string errorMsg;

            switch (code) {
                case 400:
                    errorMsg = "Bad request (invalid json, etc)";
                    break;

                case 404:
                    errorMsg = "Model or resource not found";
                    break;
                
                case 429:
                    errorMsg = "Rate limit reached";
                    break;

                case 500:
                    errorMsg = "Internal server error";
                    break;

                case 502:
                    errorMsg = "Bad Gateway (cloud model or upstream service could not be reached)";
                    break;
                
                default:
                    errorMsg = "unknown error: view ollama's error message below";
                    break;
            }

            return errorMsg;
        }


    public:

    OllamaClient();
    // OllamaClient(std::string baseAdd, httplib::Client client)
    const OllamaRes generate(const GenRequest& req);
    // const OllamaRes OllamaClient::generateStream(const GenRequest& req);
    const OllamaRes listModels();
    const OllamaRes create(const ModelConfig& config);
    const OllamaRes remove(std::string model);
       

};

} //ladybug namespace

#endif