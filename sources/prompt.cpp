
#include "prompt.hpp"


prompt::prompt(std::vector<std::string> env)
{
    _env = env;
    _prompt_aliases["\\u"] = [this](std::vector<std::string> args) { return getUsername(); };
    _prompt_aliases["\\h"] = [this](std::vector<std::string> args) { return getHostname(); };
    _prompt_aliases["\\w"] = [this](std::vector<std::string> args) { return getCwd(); };
    _prompt_aliases["\\W"] = [this](std::vector<std::string> args) { return getMinimalCwd(); };
    _prompt_aliases["\\d"] = [this](std::vector<std::string> args) { return getDate(); };
    _prompt_aliases["\\t"] = [this](std::vector<std::string> args) { return getTime(); };
    _prompt_aliases["\\$"] = [this](std::vector<std::string> args) { return getStatus(); };
}

prompt::~prompt()
{
}

std::string prompt::getHostname()
{
    char hostname[1024];
    gethostname(hostname, 1024);
    return std::string(hostname);
}

std::string prompt::getUsername()
{
    for (int i = 0; i < _env.size(); i++) {
        if (_env[i].find("USER=") != std::string::npos) {
            return _env[i].substr(5);
        }
    }
    return "unknown";
}

std::string prompt::getCwd()
{
    char cwd[1024];
    getcwd(cwd, 1024);
    return std::string(cwd);
}

std::string prompt::getDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string date = std::to_string(1900 + ltm->tm_year) + "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday);
    return date;
}

std::string prompt::getTime()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string time = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
    return time;
}

std::string prompt::getMinimalCwd()
{
    std::string cwd = getCwd();
    size_t found = cwd.find_last_of("/");
    return cwd.substr(found + 1);
}

std::string prompt::getStatus()
{
    return std::to_string(status);
}

void prompt::createPrompt(std::string prompt_cmd)
{
    prompt_str = prompt_cmd;
    for (int i = 0; i < prompt_str.size(); i++) {
        if (prompt_str[i] == '\\') {
            std::string alias = prompt_str.substr(i, 2);
            if (_prompt_aliases.find(alias) != _prompt_aliases.end()) {
                std::string alias_value = _prompt_aliases[alias]({});
                prompt_str.replace(i, 2, alias_value);
            }
        }
    }
    std::cout << prompt_str;
}