#ifndef MODELCONFIG_HPP
#define MODELCONFIG_HPP

#include <string>
#include <optional>
#include <stdexcept>
#include "json.hpp"

//STILL NEEDS TO BE TESTED


//look at this file and determine what was put in the backlog or potential backlog like OllamaParameters, validating num_ctx being an int, etc or anything else that can be implemented later to improve design

//ModelConfig backlog
    //move JSON implementation to .cpp
    //reconsider Parameters/OllamaParameters abstraction after GenerateRequest exists
    //stricter JSON numeric-type validation
    //Ollama-specific parameter range validation
    //improve validation error messages
    //constructor/invariant design
    //all-or-nothing deserialization
    //decide unknown-key behavior
    //Parameters gets its own serializer/deserializer
    //clean up/default constructors





class ModelConfig {

    //needs
        //constructor

    private: 

        struct Parameters {
            std::optional<int> num_ctx; 
            std::optional<int> num_predict;
            //std::optional<float> temperature;
            //std::optional<float> top_p;
            //std::optional<int> top_k;
            //std::optional<double> repeat_penalty;
        };

        std::string model; //required and cannot be empty
        std::string from; //required and cannot be empty
        std::string system; //required but can be empty
        Parameters parameters; //optional may be absent
        bool stream = false; //required and the default for now

    public: 
        
        

        ModelConfig(){
        }

        ModelConfig(std::string model, std::string from, std::string system) :
             model{model}, from {from}, system{system}
        {}

        //td: remove inline and place it all on a cpp file
        friend void to_json(nlohmann::json& j, const ModelConfig& model){ //serializer

            j = nlohmann::json {
                {"model", model.model},
                {"from", model.from},
                {"system", model.system},
                {"stream", model.stream},
                {"parameters", {}}
            };

            if(model.parameters.num_ctx){
                j["parameters"]["num_ctx"] = model.parameters.num_ctx;
            }

            if(model.parameters.num_predict){
                j["parameters"]["num_predict"] = model.parameters.num_predict;
            }

            if(j["parameters"].empty()){
                j.erase("parameters");
            }
        }

        friend void from_json(const nlohmann::json& j, ModelConfig& model){ //deserializerr

            //td bl: think about using OllamaParam as its own class with its own to and from json down the line
            j.at("model").get_to(model.model);
            j.at("from").get_to(model.from);

            if(model.model.empty() || model.from.empty()){
                throw std::invalid_argument("field cannot be empty");
            }

            j.at("stream").get_to(model.stream);
            j.at("system").get_to(model.system);

            if(j.contains("parameters") && j["parameters"].is_object()){

                if(j["parameters"].contains("num_ctx")){
                    int numCTX = 0;
                    j.at("parameters").at("num_ctx").get_to(numCTX);
                    model.parameters.num_ctx = numCTX;
                }

                if(j["parameters"].contains("num_predict")){
                    int numPredict;
                    j.at("parameters").at("num_predict").get_to(numPredict);
                    model.parameters.num_predict = numPredict;
                }
            } else if(j.contains("parameters") && !j["parameters"].is_object()){
                throw std::invalid_argument("parameters isn't an object");
            }
        }
};




#endif