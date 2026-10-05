#ifndef GENREQUEST_HPP
#define GENREQUEST_HPP

#include <string>
#include <optional>
#include <stdexcept>
#include "json.hpp"

namespace ladybug{



struct Options{
    std::optional<int> num_ctx; 
    std::optional<int> num_predict;
    //std::optional<float> temperature;
    //std::optional<float> top_p;
    //std::optional<int> top_k;
    //std::optional<double> repeat_penalty;


    void setNumCTX(int num_ctx){
        this->num_ctx = num_ctx;
    }

    void setNumPred(int num_predict){
        this->num_predict = num_predict;
    }
};

class GenRequest {

    public:

        std::string model; //required, cannot be empty
        std::string prompt; //required, can be empty string 
        std::string system; //can be absent, if its present it cannot be empty string
        Options options; // can be absent, if present, must contain at least one option valid
        bool stream = false; //one function will support stream, one will not, for now, its defaulted to false
        bool think = false; //just false for now, will think about this later

        GenRequest(){}
        GenRequest(std::string model): model{model}{}


        void setPrompt(std::string prompt){
            this->prompt = prompt;
        }
        void setSystem(std::string system){
            this->system = system;
        }

    friend void to_json(nlohmann::json& j, const GenRequest& req){

        j = nlohmann::json {
                {"model", req.model},
                {"system", req.system},
                {"prompt", req.prompt},
                {"think", req.think},
                {"stream", req.stream},
                {"options", {}},
            };

            if(req.options.num_ctx){
                j["options"]["num_ctx"] = req.options.num_ctx;
            }

            if(req.options.num_predict){
                j["options"]["num_predict"] = req.options.num_predict;
            }

            if(j["options"].empty()){
                j.erase("options");
            }
            
            if(req.system.empty()){
                j.erase("system");
            } 

            if(j["stream"].empty()){
                j.erase("stream");
            } 

            if(j["think"].empty()){
                j.erase("think");
            } 
    }

    friend void from_json(const nlohmann::json& j, GenRequest& req){

        j.at("model").get_to(req.model);

        if(req.model.empty()){
            throw std::invalid_argument("model field cannot be empty");
        }

        //for from_json idk what that means but keep it for now

        if(j.contains("stream")){
            j.at("stream").get_to(req.stream);
        }

        if(j.contains("system")){
            j.at("system").get_to(req.system);
        }

        if(j.contains("think")){
            j.at("think").get_to(req.think);
        }
        
        if(j.contains("prompt")){
            j.at("prompt").get_to(req.prompt); // oddly enough this may never be needed, why would we convert json to a request, for now just complete the func lol
        }
        if(j.contains("options") && j["options"].is_object()){

            if(j["options"].contains("num_ctx")){
                int numCTX = 0;
                j.at("options").at("num_ctx").get_to(numCTX);
                req.options.num_ctx = numCTX;
            }

            if(j["options"].contains("num_predict")){
                int numPredict;
                j.at("options").at("num_predict").get_to(numPredict);
                req.options.num_predict = numPredict;
            }
        } else if(j.contains("options") && !j["options"].is_object()){
            throw std::invalid_argument("options isn't an object");
        }
    }

};


}

#endif