
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include "utils.hpp"

class script {
public:
    script();
    ~script();
    void setScript(std::string script_str);
    void run();
    int check_variable(std::string line);
    int for_loop(std::vector<std::string> tokens);
    int if_statement(std::vector<std::string> tokens);
    int count_tokens(std::vector<std::string> tokens);

protected:

private:
    std::string script_str;
    std::vector<std::string> parsed_script;
    std::map<std::string, std::string> _variables;
    std::map<std::string, std::function<int(std::vector<std::string>)>> _functions;
};