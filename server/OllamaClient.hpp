#ifndef OLLAMACLIENT_HPP
#define OLLAMACLIENT_HPP

#include <string>
#include <chrono>
#include <iostream>
#include "GenRequest.hpp"
#include "ModelConfig.hpp"
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

        // OllamaClient(std::string baseAdd, httplib::Client client) 


        // const OllamaRes generate(GenRequest& req)
        const OllamaRes generate(const std::string prompt){

            if(prompt.empty()){ //if empty return a new line, let the api consumer decide what he wants to do after that
                return OllamaRes("\n", false);
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
                + "Ollama response: " + res->body;
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

                } catch (const nlohmann::json::exception& ex){
                    response.response = ex.what();
                    std::cerr << response.response << std::endl;  
                }
            }
            return response;
        }

        const OllamaRes generate(const GenRequest& req) {

            nlohmann::json genReq = req;
            nlohmann::json oll_res;
            OllamaRes response;

            //http request
            auto res = client.Post("/api/generate", genReq.dump(), "application/json");

            //check response
            if(!res){
                response.response = "Connection error: " + httplib::to_string(res.error());
                std::cerr << response.response << "\n\n"; 
            } else if (res->status != 200){ 
                response.response = statusError(res->status) + '\n' 
                + "Ollama response: " + res->body;
                std::cerr << response.response << "\n\n"; 
            } else{

                try {
                    oll_res = nlohmann::json::parse(res->body);

                    //cleaner shorter but less explicit option
                    response.response = oll_res.at("response").get<std::string>();
                    response.success = !response.response.empty(); //same logic as above just more succinct as it checks if empty and based on that sets success

                } catch (const nlohmann::json::exception& ex){
                    response.response = ex.what();
                    std::cerr << response.response << std::endl;  
                }
            }
            return response;
        }

        const OllamaRes generateStream(const GenRequest& req){
            //basically the above but keep looping until status = done
            //I mean its gonna send a lot of responses, how to handle that?

            return OllamaRes{};
        }


        const OllamaRes create(const ModelConfig& config){

            //todo

            //pass in a model config
            //this modelconfig we turn into json using to_json
            nlohmann::json req = config; //store it 
            nlohmann::json oll_res;

            auto res = client.Post("/api/create", req.dump(), "application/json");

            OllamaRes response;

            if(!res){
                response.response = "Connection error: " + httplib::to_string(res.error());
                std::cerr << response.response << "\n\n"; 
            } else if (res->status != 200){ 
                response.response = statusError(res->status) + '\n' 
                + "Ollama response: " + res->body;
                std::cerr << response.response << "\n\n"; 
            } else{

                try {
                    oll_res = nlohmann::json::parse(res->body);

                    //cleaner shorter but less explicit option
                    response.response = oll_res.at("status").get<std::string>();
                    response.success = true;
                } catch (const nlohmann::json::exception& ex){
                    response.response = ex.what();
                    std::cerr << response.response << std::endl;  
                }
            }
            return response;

        }

         const std::string remove(std::string model){

            //todo


            return "model deleted\n";
        }

        // const ModelConfig show(std::string model){
        //     show api
        // }

};

#endif