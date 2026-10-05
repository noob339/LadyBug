#ifndef LADYBUGSERVER_HPP
#define LADYBUGSERVER_HPP

#include "json.hpp"
#include "httplib.h"
#include "ladybug/OllamaClient.hpp"
#include "ladybug/ModelConfig.hpp"
#include "ladybug/GenRequest.hpp"

//this is where we implement the server that the front end uses to call it to be able to generate or create or any other endpoint 

//I would like to do validation for the functions in OllamaClient
//then harden the server
//then continue from there and look at it all
        //change all res content to json eventually, deal with one transfer or data


namespace ladybug{

class LadyBugServer {


    private:

        httplib::Server svr;
        OllamaClient cli;
        ModelConfig config; //the actual model though, if the model changes, we change it

        void handleGeneration(const httplib::Request& req, httplib::Response& res);
        void handleModelList(const httplib::Request& req, httplib::Response& res);
        void handleModelCreation(const httplib::Request& req, httplib::Response& res);
        void handleDeleteModel(const httplib::Request& req, httplib::Response& res);
        void routes();


    public:
        LadyBugServer();
        void start();
        void stop();


};

}  //ladybug namespace

#endif

