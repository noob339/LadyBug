#ifndef LADYBUGSERVER_HPP
#define LADYBUGSERVER_HPP

#include "json.hpp"
#include "OllamaClient.hpp"
#include "ModelConfig.hpp"
#include "GenRequest.hpp"
#include "httplib.h"

class LadyBugServer {

    //this is where we implement the server that the front end uses to call it to be able to generate or create or any other endpoint 
    
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

            //1
            if(!req.has_param("model") || !req.has_param("from") ){
                res.status = 404;
                res.set_content("missing fields", "text/plain");
            }
            //maybe separate for each param missing

            //2
            nlohmann::json jReq = nlohmann::json::parse(req.body);
            
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

        //change all res content to json eventually, deal with one transfer or data

        //implement handler
        //no validation just make it work
        //

        void routes(){
            svr.Get("/generate", [this](const httplib::Request& req, httplib::Response& res){
                handleGeneration(req, res);
            });

            svr.Get("/model-list", [this](const httplib::Request& req, httplib::Response& res){
                handleModelList(req, res);
            });

            svr.Post("/create_model", [this](const auto &req, auto &res) {
                handleModelCreation(req, res);
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

#endif