#include "ladybug/ModelConfig.hpp"

namespace ladybug{

ModelConfig::ModelConfig(){}

ModelConfig::ModelConfig(std::string model, std::string from, std::string system) :
        model{model}, from {from}, system{system}
{}

ModelConfig::ModelConfig(std::string model, std::string from, std::string system, Parameters params) :
        model{model}, from {from}, system{system}, parameters{params}
{}

const std::string ModelConfig::getModel() const {
    return model;
}

//td: remove inline and place it all on a cpp file
void to_json(nlohmann::json& j, const ModelConfig& model){ //serializer

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

void from_json(const nlohmann::json& j, ModelConfig& model){ //deserializerr

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

} //ladybug namespace