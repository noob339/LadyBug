#include "ladybug/OllamaClient.hpp"

#include <string>
#include <optional>
#include <stdexcept>

#include "json.hpp"

namespace ladybug{


OllamaClient::OllamaClient(){
    client.set_connection_timeout(connectionTimeout);
    client.set_write_timeout(writeTimeout);
    client.set_read_timeout(readTimeout);
}

// OllamaClient(std::string baseAdd, httplib::Client client) {}


const OllamaRes OllamaClient::generate(const GenRequest& req) {

    nlohmann::json genReq = req;
    
    auto res = client.Post("/api/generate", genReq.dump(), "application/json");
    
    OllamaRes response;

    //all methods use this paradigm, can it be refactored to be called? you pass in a res by ref, an OllamaRes by ref
    if(!res){
        response.response = "Connection error: " + httplib::to_string(res.error());
        response.status = 500;
        std::cerr << "status code: " << response.status << "\n" << response.response << "\n\n"; 
    } else if (res->status != 200){ 
        response.response = statusError(res->status) + '\n' 
        + "Ollama response: " + res->body;
        response.status = res->status;
        std::cerr << response.response << "\n\n"; 
    } else{

        nlohmann::json oll_res;

        try {
            oll_res = nlohmann::json::parse(res->body);

            //cleaner shorter but less explicit option
            response.response = oll_res.at("response").get<std::string>();
            response.status = 200;
            response.success = !response.response.empty(); //same logic as above just more succinct as it checks if empty and based on that sets success

        } catch (const nlohmann::json::exception& ex){
            response.response = ex.what();
            response.status = 400;
            std::cerr << response.response << std::endl;  
        }
    }
    return response;
}

// const OllamaRes OllamaClient::generateStream(const GenRequest& req){
//     //basically the above but keep looping until status = done
//     //I mean its gonna send a lot of responses, how to handle that?

//     return OllamaRes{};
// }

const OllamaRes OllamaClient::listModels(){

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

const OllamaRes OllamaClient::create(const ModelConfig& config){
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


const OllamaRes OllamaClient::remove(std::string model){

    nlohmann::json req = nlohmann::json::object();
    req["model"] = model;
    
    auto res = client.Delete("/api/delete", req.dump(), "application/json");

    OllamaRes response;

    if(!res){
        response.response = "Connection error: " + httplib::to_string(res.error());
        std::cerr << response.response << "\n\n"; 
    } else if (res->status != 200){ 
        response.response = statusError(res->status) + '\n' 
        + "Ollama response: " + res->body;
        response.status = res->status;
        std::cerr << response.response << "\n\n"; 
    } else{

            response.response = "success";
            response.status = res->status;
            response.success = true;

    }

    return response;

}

} //ladybug namespace