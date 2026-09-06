#ifndef OLLAMACLIENT_HPP
#define OLLAMACLIENT_HPP

#include <string>
#include <chrono>
#include <iostream>
#include "httplib.h"
#include "json.hpp"

struct OllamaRes {

    std::string response;
    bool success;

    OllamaRes(){
        this->response = "";
        this->success = false;
    }

    OllamaRes(std::string response){
        this->response = response;
        this->success = false; //many conditions where it could be false, but only one where it can be true
    }

    OllamaRes(std::string response, bool success){
        this->response = response;
        this->success = success; //many conditions where it could be false, but only one where it can be true
    }

}; 


class OllamaClient{
    //what should the program do if parsing fails? just throw, log the error and return, it failed to parse for whatever reason, malformed json, etc


    private:
        const std::string baseAdd {"http://localhost:11434"};
        httplib::Client client{baseAdd};
        nlohmann::json defaultJson;
        std::chrono::seconds connectionTimeout{2}; 
        std::chrono::seconds writeTimeout{10};
        std::chrono::seconds readTimeout{90};


        std::string statusError (int code){

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


        OllamaClient(){

            client.set_connection_timeout(connectionTimeout);
            client.set_write_timeout(writeTimeout);
            client.set_read_timeout(readTimeout);

            //default generation settings and default model but no prompt is added, that happens later in generation
            defaultJson = nlohmann::json::parse(R"({
                    "model": "gpt-oss:120b-cloud",
                    "system": "Answer clearly and concisely.",
                    "stream": false
                })");
        }


        OllamaRes generate(std::string prompt){

            if(prompt.empty()){ //if empty return a new line, let the api consumer decide what he wants to do after that
                return OllamaRes("empty", false);
            }

            nlohmann::json genJson = defaultJson;
            genJson["prompt"] = prompt;


            nlohmann::json oll_res;
            OllamaRes response;

            //http request
            auto res = client.Post("/api/generate", genJson.dump(), "application/json");

            //check response
            if(!res){
                response.response = "Connection error: " + httplib::to_string(res.error());
                std::cerr << response.response << "\n\n"; 
            } else if (res->status != 200){ 
                response.response = statusError(res->status) + '\n' 
                + "Ollama response: " + res->body + "\n\n";
                std::cerr << response.response << "\n\n"; 
            } else{

                try {
                    oll_res = nlohmann::json::parse(res->body);

                    //cleaner shorter but less explicit option
                    response.response = oll_res.at("response").get<std::string>();
                    response.success = true;

                    if(oll_res["response"] == ""){
                        response.success = false;
                    }


                    //option 2, longer but more explicit 
                    //check if its an object
                    //check if the json has a response keyword
                    //check if that value is a string
                    //check if the response empty

                    // if(oll_res.is_object()){

                    //     if(oll_res.contains("response")){

                    //         if(oll_res["response"].is_string()){
                                
                    //             if(oll_res["response"] == ""){
                    //                 response.response = "empty";
                    //                 response.success = false;
                    //             } else{
                    //                 response.response = oll_res["response"];
                    //                 response.success = true;
                    //             }
                    //         }
                    //     }
                    // } else {
                    //     //a whole bunch of nested else for every if stating the why it failed
                    // }

                } catch (const nlohmann::json::exception& ex){
                    response.response = ex.what();
                    std::cerr << response.response << std::endl;  
                }
            }
            return response;
        }
};

#endif