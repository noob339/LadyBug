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

        GenRequest();
        GenRequest(std::string model);


        inline void setPrompt(std::string prompt){
            this->prompt = prompt;
        }

        inline void setSystem(std::string system){
            this->system = system;
        }

        friend void to_json(nlohmann::json& j, const GenRequest& req);

        friend void from_json(const nlohmann::json& j, GenRequest& req);

};


} //ladybug namespace

#endif