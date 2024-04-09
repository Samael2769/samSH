#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <map>
#include <functional>
#include <unistd.h>
#include <cstring>
#include <ctime>

class prompt {
    public:
        prompt(std::vector<std::string> env);
        ~prompt();
        std::string getHostname();
        std::string getUsername();
        std::string getCwd();
        std::string getDate();
        std::string getTime();
        std::string getMinimalCwd();
        std::string getStatus();
        void createPrompt(std::string prompt_cmd);
        void setStatus(int status) { this->status = status; }
    protected:
    private:
        std::string prompt_str;
        std::map<std::string, std::function<std::string(std::vector<std::string>)>> _prompt_aliases;
        std::vector<std::string> _env;
        int status;
};