#include <iostream>
#include "httplib.h"
#include "CrossPlatform.hpp"
#include "json.hpp"
#include <string>
#include <fstream>
#include <cstdlib>
#include <map>
#include <variant>
#include <cctype>



using json = nlohmann::json;


//helper function prototypes
std::string readFile(std::string filePath);

bool is_whitespace(char c);
void skip_whitespace(const std::string& text, size_t& index);
void skip_non_whitespace(const std::string& text, size_t& index);
void trim_end(std::string& text);
void escape_json(std::string& text);

std::string base_model(const std::string& text);
std::string substr(std::string& text, size_t start, size_t end);

bool is_int(std::string& text);
bool is_float(std::string& text);
std::map<std::string, std::string> parse_params(const std::string& text);


//Struct for ParamType
struct ParamType {
    enum param_type {null, integer, real, text} type;
    std::string description;

    ParamType(): type(param_type::null){}

    ParamType(std::string type_str, std::string desc): type(param_type::null), description(desc) 
    {
        if(type_str == "int")
            type = param_type::integer;
        else if(type_str == "float")
            type = param_type::real;
        else if(type_str == "string")
            type = param_type::text;
    }

    bool is_valid(std::string value)
    {
        switch(type) {
            case(param_type::integer):
                return is_int(value);
            case(param_type::real):
                return is_float(value);
            case(param_type::text):
                return true;
            default: return false;
        }
    }

    std::string to_string()
    { 
        switch(type) {
            case(param_type::integer):
                return "Integer";
            case(param_type::real):
                return "Float";
            case(param_type::text):
                return "String";
            default:
                return "Null";
        }
    }

    friend std::ostream& operator<<(std::ostream& o, ParamType& pt)
    {
        o << pt.to_string();
        return o;
    }
};

//I assume this method prototype must wait for the struct to be defined before being able to be declared
std::map<std::string, ParamType> parse_param_types(std::string config); 

struct BaseModel {
    std::string name, params, size;

    std::string to_string()
    {
        std::string str;
        str += "Model: ";
        str += name;
        str += ", Parameters: ";
        str += params;
        str += ", Size: ";
        str += size;
        return str;
    }
};

std::map<std::string, BaseModel> parse_base_models(std::string config);

std::string jsonify(std::map<std::string, ParamType>& ptypes);
std::string jsonify(std::map<std::string, BaseModel>& models);



struct Model {
    std::string model_name = "test";
    std::string context;
    std::string filepath;
    std::string from_model;
    std::map<std::string, std::string> parameters;
    Model(std::string filepath): filepath(filepath){}

    void to_file() const 
    {
        std::fstream file(filepath + PATH_SEPARATOR + model_name + ".conf", std::ios::out | std::ios::trunc);
        file << to_string();
    }

    std::string to_string() const //more like build_model_config
    {
        std::string text;
        text += "FROM ";
        text += from_model;
        text += "\n";

        for(auto &[key, value] : parameters)
        {
            text += "PARAMETER ";
            text += key;
            text += " ";
            text += value;
            text += "\n";
        }

        text += "\nSYSTEM \"\"\"";
        text += context;
        text += "\"\"\"\n";
        return text;
    }
    
    void load_ollama() const
    {
        std::string init_command = "ollama create ";
        init_command += model_name;
        init_command += " -f ";
        init_command += filepath;
        init_command += PATH_SEPARATOR;
        init_command += model_name;
        init_command += ".conf";
        std::cout<<init_command<<"\n";
        std::system(init_command.c_str());
    }

    void warm_up() const
    {
        run("");
    }

    std::string run(std::string prompt) const
    {
        std::string command = "ollama run test --hidethinking --nowordwrap ";
        command += "\"" + prompt + "\"";
        command += " > model_output.txt";
        std::system(command.c_str());
        return readFile("model_output.txt");
    }
};

Model load_model_conf(std::string model_name, std::string filepath);






/* My work */



class LadyBugServer {

    //this is where we implement the server that the front end uses to call it to be able to generate or create or any other endpoint 
    

};



struct GenerationRequest {

    struct options{
    };

    private:

    public:

};


struct ChatRequest {

};




//MAIN FUNCTION

int main(int argc, char** argv)
{

    std::cout << "starting server..." << std::endl;

    size_t file_index = 1;
    auto type_map = parse_param_types(readFile("./params.types"));
    auto base_models = parse_base_models(readFile("./models.list"));
    std::string model_name = argc > 2 ? argv[file_index++] : "test";
    Model model = load_model_conf("test", std::string(".."));

    model.load_ollama();
    model.warm_up();

    httplib::Server svr;
    svr.set_mount_point("/", "../client/dist");

    

    svr.Get("/query", [model](const auto &req, auto &res) {
    
        std::string response;
        if (!req.has_param("prompt")) 
        {
            response += "{\"data\":\"\", \"error\":\"Exception, must supply parameter 'prompt'.\", \"success\":false }\n";
            res.set_content(response, "text/html");
            return;
        }

        std::string prompt = req.get_param_value("prompt");
        std::cout<<prompt<<"\n";



        //🚨 lo5w or design decisions to be made here, what is worthy of keeping or scrapping?
                // here is where we would call generate
                // the problem is, its done through this model construction which honestly, may be a good idea, 
                // how do I switch the models? how do I switch the parameters
                //is anything here worth keeping or using?
            // THEN AGAIN, it could be called in Model's run function, would be the one to least break it tbh but my design conflicts this design. 
                // the logic wouldn't match the models logic, all that parsing and extracting from andrews functions for what if my json being fed is handled inside the client. 
                // welp, based on the logic presented in the google doc from chatgpt, generate goes in the model.run() function replacing the command line, 
                // so I can modify ollama client to bend it to work as the sprint originally intended
                // a lot of it seems useless on close examination. This was wired to work with the command line above all else, Some I can def use between the react client and the server but some of it might need to go
                // when we send a prompt we are sending it with this pre loaded test config that has our options,
                // how would I translate that to ollamaclient



        std::string model_output = model.run(prompt);
        trim_end(model_output);

        res.set_content(
            model_output,
            "text/plain; charset=UTF-8"
        );

        std::cout<<"responding to prompt:\n"<<prompt<<"\nwith output:\n"<<model_output<<"\n";
    });


    svr.Get("/set_context", [&model](const auto &req, auto &res) {
        if(!req.has_param("ctx"))
        {
            res.set_content("{ \"error\":\"must have param ctx for new model context.\", \"success\":false }\n", "text/json");
            return;
        }
        model.context = req.get_param_value("ctx");
        model.to_file();
        model.load_ollama();
        model.warm_up();
        res.set_content("{\"error\":\"\", \"success\":true}\n", "text/json");
    });


    svr.Get("/set_parameter", [&model, &type_map](const auto &req, auto &res) {
        if(!req.has_param("key") || !req.has_param("value"))
        {
            res.set_content("{\"error\":\"must include parameters key for parameter name, and value for value.\", \"success\":false }\n", "text/json");
            return;
        }
        if(type_map.count(req.get_param_value("key")) == 0) 
        {
            res.set_content("{\"error\":\"Error, invalid key: \'" + req.get_param_value("key") + "\'\", \"success\":false }\n", "text/json");
            return;
        }
        std::string key = req.get_param_value("key");
        std::string value = req.get_param_value("value");
        if(!type_map[key].is_valid(value))
        {
            res.set_content("{\"error\":\"Error, invalid value: \'" + value + "\' for parameter: \'" + key + "\' of type: " + type_map[key].to_string() + "\", \"success\":false }\n", "text/json");
            return;
        }

        std::cout << "updating param: " << key << " which is of type: " << type_map[key] << " with value: " << value << "\n";
        model.parameters[req.get_param_value("key")] = req.get_param_value("value");
        
        model.to_file();
        model.load_ollama();
        model.warm_up();

        res.set_content("{\"error\":\"\", \"success\":true}\n", "text/json");
    });

    svr.Get("/set_base_model", [&model, &base_models](const auto &req, auto &res) {
        if(!req.has_param("model"))
        {
            std::string resp = "{\"error\":\"";
            resp += "must include parameter model";
            resp += "\", \"success\":false}";
            res.set_content(resp, "text/json");
            return;
        }
        std::string model_name = req.get_param_value("model");
        if(base_models.count(model_name) == 0)
        {
            std::string resp = "{\"error\":\"";
            resp += "Error, invalid model name: \'";
            resp += model_name;
            resp += "\'";
            resp += "\", \"success\":false}";
            res.set_content(resp, "text/json");
            return;
        }
        
        model.from_model = std::move(model_name);
        model.to_file();
        model.load_ollama();
        model.warm_up();
        
        res.set_content(std::string("{\"info\":\"") + base_models[model.from_model].to_string() + "\", \"error\":\"\", \"success\":true}\n", "text/json");
    });


    svr.Get("/base_model_options", [&base_models](const auto &req, auto &res) {
        res.set_content(jsonify(base_models), "text/json");
    });


    svr.Get("/model_parameter_options", [&type_map](const auto &req, auto &res) {
        res.set_content(jsonify(type_map), "text/json");
    });

    std::cout << "Server running http://localhost:8080/." << std::endl;
    svr.listen("0.0.0.0", 8080);
}




bool is_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

void skip_whitespace(const std::string& text, size_t& index)
{
    while(text.size() > index && is_whitespace(text[index])) index++;
}

void skip_non_whitespace(const std::string& text, size_t& index)
{
    while(text.size() > index && text[index] != ' ' && text[index] != '\t' && text[index] != '\n') index++;
}

void trim_end(std::string& text)
{
    size_t last = 0, cur = 0;

    while(cur < text.size())
    {
        skip_non_whitespace(text, cur);
        last = cur;

        do {
            cur++;
            skip_whitespace(text, cur);
        } while(text[cur] == '\n');
    }

    text = text.substr(0, last);
}

void escape_json(std::string& text)
{
    std::string output;
    for(char c : text)
    {
        switch(c) {
            case('\"'):
                output += "\\\"";
                break;
            case('\\'):
                output += "\\\\";
                break;
            case('\b'):
                output += "\\b";
                break;
            case('\f'):
                output += "\\f";
                break;
            case('\n'):
                output += "\\n";
                break;
            case('\r'):
                output += "\\r";
                break;
            case('\t'):
                output += "\\t";
                break;
            default:
                output += c;
        }
    }
    text = output;
}

std::string base_model(const std::string& text)
{
    size_t start_name = text.find("FROM");
    if(start_name == std::string::npos)
        return std::string();

    start_name += strlen("FROM");
    skip_whitespace(text, start_name);
    size_t end_name = start_name;
    skip_non_whitespace(text, end_name);
    return text.substr(start_name, end_name - start_name);
}

bool is_int(std::string& text)
{
    size_t i = text[0] == '-';

    if(i >= text.size())
            return false;

    bool is_int = true;

    while(i < text.size() && is_int) 
        is_int = isdigit(text[i++]);

    return is_int;
}

bool is_float(std::string& text)
{
    size_t i = text[0] == '-';

    if(i >= text.size())
        return false;

    bool is_float = true, is_beg = true;

    while(i < text.size() && is_float)
    {
        if(is_beg && text[i] == '.')
        {
            is_beg = false;
        }
        else if(!isdigit(text[i]))
            is_float = false;
        i++;
    }

    return is_float;
}

std::string substr(std::string& text, size_t start, size_t end)
{
    return text.substr(start, end - start);
}

std::map<std::string, ParamType> parse_param_types(std::string config)
{
    std::map<std::string, ParamType> types;
    size_t i = 0;
    while(i < config.size())
    {
        const size_t name_start = i;
        skip_non_whitespace(config, i);
        const size_t name_end = i;
        skip_whitespace(config, i);
        
        const size_t type_start = i;
        skip_non_whitespace(config, i);
        const size_t type_end = i;

        skip_whitespace(config, i);
        
        if(i >= config.size() || config[i] == '\n')
        {
            i++; 
            types[substr(config, name_start, name_end)] =
                    ParamType(substr(config, type_start, type_end), "");
            continue;
        }
        
        const size_t desc_start = i;
        
        while(i < config.size() && config[i] != '\n') i++;

        const size_t desc_end = i++;
        std::string type = substr(config, type_start, type_end);
        std::string desc = substr(config, desc_start, desc_end);
        ParamType pt = ParamType(type, desc);
        types[substr(config, name_start, name_end)] = pt;
                    
    }

    return types;
}

std::map<std::string, std::string> parse_params(const std::string& text)
{
    size_t index = 0;
    std::map<std::string, std::string> params;
    while(index < text.size())
    {
        index = text.find("PARAMETER", index);
        if(index == std::string::npos)
                break;
        index += strlen("PARAMETER");
        skip_whitespace(text, index);
        const size_t start_key = index;
        skip_non_whitespace(text, index);
        if(index >= text.size())
                break;
        const size_t end_key = index;
        skip_whitespace(text, index);
        if(index >= text.size())
                break;
        const size_t start_value = index;
        skip_non_whitespace(text, index);  
        if(index >= text.size())
                break;
        params[text.substr(start_key, end_key - start_key)] = text.substr(start_value, index - start_value);
    }
    
    return params;
}

std::map<std::string, BaseModel> parse_base_models(std::string config)
{
    size_t i = 0;
    std::map<std::string, BaseModel> map;
    while(i < config.size())
    {
        size_t name_s = i;
        skip_non_whitespace(config, i);
        size_t name_e = i;
        
        skip_whitespace(config, i);
        
        size_t param_s = i;
        skip_non_whitespace(config, i);
        size_t param_e = i;
        
        skip_whitespace(config, i);

        size_t size_s = i;
        skip_non_whitespace(config, i);
        size_t size_e = i;
        
        skip_whitespace(config, i);

        BaseModel model;
        model.name = substr(config, name_s, name_e);
        model.params = substr(config, param_s, param_e);
        model.size = substr(config, size_s, size_e);
        map[model.name] = model;
        i = config.find('\n', i);
        i++;
    }
        
    return map;
}

std::string jsonify(std::map<std::string, ParamType>& ptypes)
{
    std::string json;
    json += "[";

    for(auto [name, ptype] : ptypes)
    {
        json += "{\"param_name\":\"";
        json += name;
        json += "\", \"type\":\"";
        json += ptype.to_string();
        json += "\", \"desc\":\"";
        json += ptype.description;
        json += "\"},\n";
    }

    if(ptypes.size() > 0)
        json[json.size() - 2] = ' ';
        
    json += "]\n";

    return json;
}

std::string jsonify(std::map<std::string, BaseModel>& models)
{
    std::string json;
    json += "[";

    for(auto [name, model] : models)
    {
        json += "{\"model_name\":\"";
        json += name;
        json += "\", \"params\":\"";
        json += model.params;
        json += "\", \"size\":\"";
        json += model.size;
        json += "\"},\n";
    }

    if(models.size() > 0)
            json[json.size() - 2] = ' ';

    json += "]\n";
    
    return json;
}

std::string readFile(std::string filePath)
{
    std::fstream file = std::fstream(filePath);
    std::string val((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return val;
}

Model load_model_conf(std::string model_name, std::string filepath)
{
    std::string text = readFile(filepath + PATH_SEPARATOR + model_name + ".conf");
    std::cout<<"loading conf file: "<<(filepath + PATH_SEPARATOR + model_name + ".conf")<<"\n";
    size_t end_of_initial = text.find("SYSTEM");
    if(end_of_initial == std::string::npos)
            end_of_initial = text.size();
    Model model(filepath);
    model.model_name = std::move(model_name);
    model.from_model = base_model(text);
    model.parameters = parse_params(text);

    do {

        if(end_of_initial < text.size())
        {
            size_t start_of_ctx = text.find("\"\"\"", end_of_initial);
            bool triple_quotes = true;
            if(start_of_ctx == std::string::npos)
            {
                    triple_quotes = false;
                    start_of_ctx = text.find("\"");
                    if(start_of_ctx != std::string::npos)
                            start_of_ctx++;
            }
            else
                    start_of_ctx += 3;
            if(start_of_ctx == std::string::npos)
                    break;

            size_t end_of_ctx = text.find(triple_quotes ? "\"\"\"" : "\"", start_of_ctx);
            if(end_of_ctx == std::string::npos)
                    break;
            model.context = text.substr(start_of_ctx, end_of_ctx - start_of_ctx);
        }

    } while(false);

    std::cout<<"loaded model:\n"<<model.to_string()<<"\n";

    return model;
}






