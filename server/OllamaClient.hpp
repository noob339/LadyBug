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

        const OllamaRes generate(const GenRequest& req) {

            nlohmann::json genReq = req;
            nlohmann::json oll_res;
            OllamaRes response;

            //http request
            auto res = client.Post("/api/generate", genReq.dump(), "application/json");

            //check response
            if(!res){
                response.response = "Connection error: " + httplib::to_string(res.error());
                response.status = 500;
                std::cerr << "status code: " << response.status << "\n" << response.response << "\n\n"; 
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

        // const OllamaRes generateStream(const GenRequest& req){
        //     //basically the above but keep looping until status = done
        //     //I mean its gonna send a lot of responses, how to handle that?

        //     return OllamaRes{};
        // }

        const OllamaRes listModels(){

            //the path from server <-> ollamaclient is not hardened or verified. Need to validate data here and at Server need to handle that as well. I am seeing that event driven clicks don't really send a req, more like just asking send whatever should be sent if I click here
            //incorporate status

            nlohmann::json req;

            nlohmann::json oll_res_models;
            OllamaRes response;

            auto res = client.Get("/api/tags");

            if(!res){
                response.response = "Connection error: " + httplib::to_string(res.error());
                response.status = 500;
                std::cerr << "status code: " << response.status << "\n" << response.response << "\n\n"; 
            }else if (res->status != 200){ 
                response.response = statusError(res->status) + '\n' 
                + "Ollama response: " + res->body;
                std::cerr << response.response << "\n\n"; 
            } else{
                try {
                    oll_res_models = nlohmann::json::parse(res->body);
                    nlohmann::json model_list = nlohmann::json::array();

                    // validation of data is still needed
                    //you have to ensure that models exists? if it doesn't there is an issue
                    // if(!oll_res_models.contains("models")){

                    for(const auto & mod : oll_res_models["models"]){
                        std::string name = mod["name"].get<std::string>();
                        std::string model = mod["model"].get<std::string>();
                        std::string parent = mod["details"]["parent_model"].get<std::string>();

                        model_list.push_back({
                            {"name", name},
                            {"model", model},
                            {"parent", parent}
                        });
                    }
                    response.response = model_list.dump(); 
                    response.success = true;
                } catch (const nlohmann::json::exception& ex){
                    response.response = ex.what();
                    std::cerr << response.response << std::endl;  
                }
            }
            return response;
        }

        const OllamaRes create(const ModelConfig& config){
            //todo
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
                    response.response = oll_res.at("status").get<std::string>(); //"success"
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