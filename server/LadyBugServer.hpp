#ifndef LADYBUGSERVER_HPP
#define LADYBUGSERVER_HPP

#include "json.hpp"
#include "OllamaClient.hpp"
#include "ModelConfig.hpp"
#include "GenRequest.hpp"
#include "httplib.h"

namespace ladybug{

class LadyBugServer {

    //this is where we implement the server that the front end uses to call it to be able to generate or create or any other endpoint 

    //I would like to do validation for the functions in OllamaClient
    //then harden the server
    //then continue from there and look at it all
            //change all res content to json eventually, deal with one transfer or data
    
    private:

        httplib::Server svr;
        OllamaClient cli;
        ModelConfig config; //the actual model though, if the model changes, we change it

        void handleGeneration(const httplib::Request& req, httplib::Response& res){
            //parse
            //validate
            //assign
            //process
            //get result
            //assign response = result
            //done


            //any headers I should be sending
            //should status be set right away?

            nlohmann::json jReq = nlohmann::json::parse(req.body);
            OllamaRes resGen;

            if (!jReq.contains("prompt")) {
                resGen.response += "{\"data\":\"\", \"error\":\"Exception, must supply parameter 'prompt'.\", \"success\":false }\n";
                res.status = 400; //make a method for the most needed ones
                res.set_content(resGen.response, "text/html");
                return;
            }
            
            GenRequest genReq = jReq;
            resGen = this->cli.generate(genReq);

            std::string model_output;

            if(resGen.success){
                model_output = resGen.response;
                res.status = 200;
                res.set_content(
                    model_output,
                    "text/plain; charset=UTF-8"
                );

                std::cout << "DEBUG TEST: " << model_output << std::endl;
            }
        }

        void handleModelList(const httplib::Request& req, httplib::Response& res){
            
            
            //are ollama errors technically ladybug errors?
            //think about it
            //should the error client get propaagate up? Yes it should lol
            
            OllamaRes models;
            
            try {
                 models = this->cli.listModels();

            } catch(std::exception e){
                models.response = e.what();
                res.status = 400;
                return;
            }
            
            
            if(models.success){
                res.status = 200;
                res.set_content(models.response, "application/json");
            }

            //maybe oll res should send status code too
        }

        void handleModelCreation(const httplib::Request& req, httplib::Response& res){
            //1. check if req is valid or received aka validate
            //2. parse
            //3. assign validate again? na im sure not
            //4. process
            //5. get result
            //done

            //not robust but should work

            nlohmann::json jReq = nlohmann::json::parse(req.body);

            //1
            if(!jReq.contains("model") || !jReq.contains("from") ){
                res.status = 404;
                res.set_content("missing fields", "text/plain");
            }
            //maybe separate for each param missing
       
            
            //3
            ModelConfig config = jReq;

            //4 //5
            OllamaRes creationRes = this->cli.create(config);

            //6
            if (creationRes.success){
                res.status = 200;
                res.set_content(creationRes.response, "text/plain"); // the string should be success"
            } else {
                res.status = 400;
                res.set_content("bad request", "text/plain");
            }
        }

        void handleDeleteModel(const httplib::Request& req, httplib::Response& res){

            nlohmann::json jReq = nlohmann::json::parse(req.body);
            
            if(!jReq.contains("model") ){
                res.status = 404;
                res.set_content("missing model name", "text/plain");
            }

            

            std::string config = jReq.at("model").get<std::string>();


            OllamaRes deletionRes = this->cli.remove(config);

            if (deletionRes.success){
                res.status = deletionRes.status;
                res.set_content(deletionRes.response, "text/plain"); 
            } else {
                res.status = 400;
                res.set_content("unable to delete", "text/plain");
            }

        }


        void routes(){
            svr.Post("/generate", [this](const httplib::Request& req, httplib::Response& res){
                handleGeneration(req, res);
            });

            svr.Get("/model-list", [this](const httplib::Request& req, httplib::Response& res){
                handleModelList(req, res);
            });

            svr.Post("/create_model", [this](const auto &req, auto &res) {
                handleModelCreation(req, res);
            });

            svr.Delete("/delete", [this](const auto &req, auto &res){
                handleDeleteModel(req, res);
            });
        }


    public:


        LadyBugServer(){
            routes();
        }

        void start(){
            std::cout << "Server running http://localhost:8080/." << std::endl;
            svr.listen("0.0.0.0", 8080);
        }

        void stop(){
            svr.stop(); //will this cause issue, yihirose said it must be called from a separate thread
        }


};

}

#endif

