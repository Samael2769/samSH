/*
** EPITECH PROJECT, 2022
** Untitled (Workspace)
** File description:
** samsh
*/

#include "samsh.hpp"

samsh::samsh() {
    setEnv();
    _builtins["exit"] = std::bind(&samsh::builtinExit, this, std::placeholders::_1);
    _builtins["cd"] = std::bind(&samsh::builtinCd, this, std::placeholders::_1);
    _builtins["env"] = std::bind(&samsh::builtinEnv, this, std::placeholders::_1);
    _builtins["setenv"] = std::bind(&samsh::builtinSetenv, this, std::placeholders::_1);
    _builtins["unsetenv"] = std::bind(&samsh::builtinUnsetenv, this, std::placeholders::_1);
    _builtins["getenv"] = std::bind(&samsh::builtinGetenv, this, std::placeholders::_1);
    _builtins["echo"] = std::bind(&samsh::builtinEcho, this, std::placeholders::_1);
}

samsh::~samsh() {
}

void samsh::setEnv() {
    char **env = environ;
    for (int i = 0; env[i] != nullptr; i++) {
        _env.push_back(strdup(env[i])); // Duplicate environment strings
    }
}

int samsh::run() {
    std::string input;
    while (true) {
        setPath();
        input.clear();
        std::cout << "samsh$ ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        parse(input);
    }
    return _status;
}

std::string samsh::getEnv(std::string var) {
    for (const auto& env : _env) {
        size_t pos = env.find('=');
        if (pos != std::string::npos) {
            std::string token = env.substr(0, pos);
            if (token == var) {
                return env.substr(pos + 1);
            }
        }
    }
    return "";
}

void samsh::setPath() {
    std::string path = getEnv("PATH");
    _path = splitString(path, ':'); // Split path string by colon delimiter
}

int samsh::parse(std::string input) {
    _lastargs = splitString(input, ' '); // Split input string by space delimiter
    exec(_lastargs[0]);
    return 0;
}

int samsh::exec(const std::string& cmd) {
    pid_t pid;
    int status;
    std::string path;
    if (isBuiltin(cmd, _lastargs)) {
        return 0;
    }
    if (access(cmd.c_str(), F_OK) == 0) {
        path = cmd;
    } else
        path = findPath(cmd);
    _lastargs[0] = path; // Replace command with full path
    char **args = vectorToCharPointerArray(_lastargs);
    char **env = vectorToCharPointerArray(_env);

    if (args == nullptr) {
        std::cerr << "Failed to allocate memory for arguments" << std::endl;
        return -1;
    }
    if (path.empty()) {
        std::cerr << "Command not found" << std::endl;
        freeCharPointerArray(args); // Free memory allocated for arguments
        freeCharPointerArray(env); // Free memory allocated for environment
        return -1;
    }
    pid = fork();
    if (pid == 0) {
        if (execve(path.c_str(), args, env) == -1) {

            std::cerr << "execve failed" << std::endl;
            std::cout << strerror(errno) << std::endl;
            exit(1);
        }
    } else if (pid < 0) {
        std::cerr << "fork failed" << std::endl;
        freeCharPointerArray(args); // Free memory allocated for arguments
        freeCharPointerArray(env); // Free memory allocated for environment
        return -1;
    } else {
        wait(&status);
        _status = WEXITSTATUS(status);
        freeCharPointerArray(args); // Free memory allocated for arguments
        freeCharPointerArray(env); // Free memory allocated for environment
    }
    return 0;
}

// Split a string by a delimiter and return a vector of tokens
std::vector<std::string> samsh::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string samsh::findPath(const std::string& cmd) {
    for (const auto& dir : _path) {
        std::string path = dir + "/" + cmd;
        if (isBinaryFile(path)) {
            return path;
        }
    }
    return "";
}

bool samsh::isBuiltin(const std::string& cmd, std::vector<std::string> args) {
    if (_builtins.find(cmd) != _builtins.end()) {
        _status = _builtins[cmd](args);
        return true;
    }
    return false;
}

int samsh::builtinExit(std::vector<std::string> args) {
    if (args.size() > 1) {
        std::cerr << "exit: Expression Syntax." << std::endl;
        return -1;
    }
    exit(0);
}

int samsh::builtinCd(std::vector<std::string> args) {
    if (args.size() > 2) {
        std::cerr << "cd: Expression Syntax." << std::endl;
        return -1;
    }
    if (args.size() == 1) {
        chdir(getEnv("HOME").c_str());
    } else {
        setenv("OLDPWD", getEnv("PWD"), 1);
        if (chdir(args[1].c_str()) == -1) {
            std::cerr << "cd: " << args[1] << ": No such file or directory." << std::endl;
            return -1;
        }
        std::string path = getcwd(nullptr, 0);
        setenv("PWD", path, 1);
    }
    return 0;
}

int samsh::builtinEnv(std::vector<std::string> args) {
    if (args.size() > 1) {
        std::cerr << "env: Expression Syntax." << std::endl;
        return -1;
    }
    printEnv();
    return 0;
}

int samsh::builtinSetenv(std::vector<std::string> args) {
    if (args.size() < 2 || args.size() > 3) {
        std::cerr << "setenv: Expression Syntax." << std::endl;
        return -1;
    }
    if (args.size() == 2) {
        setenv(args[1].c_str(), "", 1);
    } else {
        setenv(args[1].c_str(), args[2].c_str(), 1);
    }
    return 0;
}

int samsh::builtinUnsetenv(std::vector<std::string> args) {
    if (args.size() != 2) {
        std::cerr << "unsetenv: Expression Syntax." << std::endl;
        return -1;
    }
    unsetenv(args[1].c_str());
    return 0;
}

int samsh::builtinGetenv(std::vector<std::string> args) {
    if (args.size() != 2) {
        std::cerr << "getenv: Expression Syntax." << std::endl;
        return -1;
    }
    std::cout << getEnv(args[1]) << std::endl;
    return 0;
}

int samsh::builtinEcho(std::vector<std::string> args) {
    for (int i = 1; i < args.size(); i++) {
        std::cout << args[i];
        if (i != args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}

int samsh::setenv(const std::string& name, const std::string& value, int overwrite) {
    if (overwrite) {
        for (int i = 0; i < _env.size(); i++) {
            size_t pos = _env[i].find('=');
            if (pos != std::string::npos) {
                std::string token = _env[i].substr(0, pos);
                if (token == name) {
                    _env[i] = name + "=" + value;
                    return 0;
                }
            }
        }
    }
    _env.push_back(name + "=" + value);
    return 0;
}

int samsh::unsetenv(const std::string& name) {
    for (int i = 0; i < _env.size(); i++) {
        size_t pos = _env[i].find('=');
        if (pos != std::string::npos) {
            std::string token = _env[i].substr(0, pos);
            if (token == name) {
                _env.erase(_env.begin() + i);
                return 0;
            }
        }
    }
    return 0;
}