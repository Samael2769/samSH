
#include "../includes/script.hpp"

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

    parse(parsed_script);
    //print variables
    std::cout << "Variables:" << std::endl;
    for (std::map<std::string, std::string>::iterator it = _variables.begin(); it != _variables.end(); it++) {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}

int script::parse(std::vector<std::string> parsed_script) {
    for (int i = 0; i < parsed_script.size(); i++) {
        parsed_script[i] = trim(parsed_script[i]);
        for (std::map<std::string, std::function<int(std::vector<std::string>)>>::iterator it = _functions.begin(); it != _functions.end(); it++) {
            if (parsed_script[i].find(it->first) != std::string::npos) {
                int end = count_tokens(std::vector<std::string>(parsed_script.begin() + i, parsed_script.end()));
                std::vector<std::string> sub_tokens;
                for (int j = i; j < end + i; j++) {
                    sub_tokens.push_back(parsed_script[j]);
                }
                it->second(sub_tokens);
                i += end - 1;
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
    return 0;
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
    std::string statement = "";
    //get only what inside the parenthesis
    for (int i = 0; i < tokens[0].size(); i++) {
        if (tokens[0][i] == '(') {
            i++;
            while (tokens[0][i] != ')') {
                statement += tokens[0][i];
                i++;
            }
            break;
        }
    }
    std::vector<std::string> for_tokens;
    for_tokens = splitString(statement, ';');
    int start = std::stoi(for_tokens[0]);
    int end = std::stoi(for_tokens[1]);
    int step = std::stoi(for_tokens[2]);
    for (int i = start; i < end; i += step) {
        std::vector<std::string> sub_tokens;
        for (int j = 1; j < tokens.size() - 1; j++) {
            sub_tokens.push_back(tokens[j]);
        }
        parse(sub_tokens);
    }
    return 0;
}

int script::if_statement(std::vector<std::string> tokens) {
    std::string statement = "";
    //get only what inside the parenthesis
    for (int i = 0; i < tokens[0].size(); i++) {
        if (tokens[0][i] == '(') {
            i++;
            while (tokens[0][i] != ')') {
                statement += tokens[0][i];
                i++;
            }
            break;
        }
    }
    if (evaluate(statement)) {
        std::vector<std::string> sub_tokens;
        for (int i = 1; i < tokens.size() - 1; i++) {
            sub_tokens.push_back(tokens[i]);
        }
        parse(sub_tokens);
    }
    return 0;
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

bool script::evaluate(std::string expression) {
    std::vector<std::string> tokens;
    tokens = splitString(expression, ' ');
    if (tokens.size() != 3) {
        return false;
    }
    if (tokens[1] == ">") {
        return superior(tokens[0], tokens[2]);
    }
    if (tokens[1] == ">=") {
        return superior_or_equal(tokens[0], tokens[2]);
    }
    if (tokens[1] == "==") {
        return equal(tokens[0], tokens[2]);
    }
    if (tokens[1] == "<") {
        return superior(tokens[2], tokens[0]);
    }
    if (tokens[1] == "<=") {
        return superior_or_equal(tokens[2], tokens[0]);
    }
    return false;
}

bool script::superior(std::string a, std::string b) {
    std::string var_a = getVariableValue(a);
    std::string var_b = getVariableValue(b);

    if (var_a == "")
        var_a = a;
    if (var_b == "")
        var_b = b;
    if (var_a > var_b) {
        return true;
    }
    return false;
}

bool script::superior_or_equal(std::string a, std::string b) {
    std::string var_a = getVariableValue(a);
    std::string var_b = getVariableValue(b);
    
    if (var_a == "")
        var_a = a;
    if (var_b == "")
        var_b = b;
    if (var_a >= var_b) {
        return true;
    }
    return false;
}

bool script::equal(std::string a, std::string b) {
    std::string var_a = getVariableValue(a);
    std::string var_b = getVariableValue(b);

    if (var_a == "")
        var_a = a;
    if (var_b == "")
        var_b = b;
    if (var_a == var_b) {
        return true;
    }
    return false;
}

std::string script::getVariableValue(std::string var) {
    if (_variables.find(var) != _variables.end()) {
        return _variables[var];
    } else {
        return "";
    }
}