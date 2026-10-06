#ifndef MODELCONFIG_HPP
#define MODELCONFIG_HPP

#include <string>
#include <optional>
#include <stdexcept>

#include "json.hpp"

namespace ladybug{

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
    //roll out the other parameters ... dang lol, this a lot but good practice 

    struct Parameters {
        std::optional<int> num_ctx; 
        std::optional<int> num_predict;
        //std::optional<float> temperature;
        //std::optional<float> top_p;
        //std::optional<int> top_k;
        //std::optional<double> repeat_penalty;
    };



class ModelConfig {

    //needs
        //constructor

    private: 

        std::string model; //required and cannot be empty
        std::string from; //required and cannot be empty
        std::string system; //required but can be empty
        Parameters parameters; //optional may be absent
        bool stream = false; //required and the default for now

    public: 

        ModelConfig();
        ModelConfig(std::string model, std::string from, std::string system);
        ModelConfig(std::string model, std::string from, std::string system, Parameters params);

        const std::string getModel() const ;
        friend void to_json(nlohmann::json& j, const ModelConfig& model);
        friend void from_json(const nlohmann::json& j, ModelConfig& model);
};

} //ladybug namespace


#endif