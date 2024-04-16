
#include "script.hpp"

script::script() {
    _functions["for"] = std::bind(&script::for_loop, this, std::placeholders::_1);
    _functions["if"] = std::bind(&script::if_statement, this, std::placeholders::_1);
}

script::~script() {
    //dtor
}

void script::setScript(std::string script_str) {
    this->script_str = script_str;
}

void script::run() {
    std::string line;
    std::istringstream iss(script_str);
    while (std::getline(iss, line)) {
        parsed_script.push_back(line);
    }
    for (int i = 0; i < parsed_script.size(); i++) {
        parsed_script[i] = trim(parsed_script[i]);
        for (std::map<std::string, std::function<int(std::vector<std::string>)>>::iterator it = _functions.begin(); it != _functions.end(); it++) {
            if (parsed_script[i].find(it->first) != std::string::npos) {
                //end = count_tokens(parsed_script + i);
                int end = count_tokens(std::vector<std::string>(parsed_script.begin() + i, parsed_script.end()));
                std::vector<std::string> sub_tokens;
                for (int j = i; j < end + i; j++) {
                    sub_tokens.push_back(parsed_script[j]);
                }
                it->second(sub_tokens);
                i += end;
            }
        }
        if (parsed_script[i].find("=") != std::string::npos) {
            if (check_variable(parsed_script[i]) != 0) {
                continue;
            }
            std::vector<std::string> tokens;
            tokens = splitString(parsed_script[i], '=');
            _variables[tokens[0]] = tokens[1];
        }
    }

    //print variables
    for (std::map<std::string, std::string>::iterator it = _variables.begin(); it != _variables.end(); it++) {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}

int script::check_variable(std::string line) {
    for (std::map<std::string, std::function<int(std::vector<std::string>)>>::iterator it = _functions.begin(); it != _functions.end(); it++) {
        if (line.find(it->first) != std::string::npos) {
            return 1;
        }
    }
    std::vector<std::string> tokens;
    tokens = splitString(line, '=');
    if (tokens.size() != 2) {
        return 1;
    }
    if (tokens[0].find("\"") != std::string::npos || tokens[0].find("\'") != std::string::npos) {
        return 1;
    }
    return 0;
}

int script::for_loop(std::vector<std::string> tokens) {
    for (int i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i] << std::endl;
    }
    //TODO
}

int script::if_statement(std::vector<std::string> tokens) {
    for (int i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i] << std::endl;
    }
    //TODO
}

int script::count_tokens(std::vector<std::string> tokens) {
    int count = 0;
    for (int i = 0; i < tokens.size(); i++) {
        for (std::map<std::string, std::function<int(std::vector<std::string>)>>::iterator it = _functions.begin(); it != _functions.end(); it++) {
            if (tokens[i].find(it->first) != std::string::npos) {
                count++;
            }
        }
        if (tokens[i].find("end") != std::string::npos) {
            count--;
        }
        if (count <= 0) {
            return i + 1;
        }
    }
    return -1;
}