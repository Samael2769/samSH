/*
** EPITECH PROJECT, 2022
** Untitled (Workspace)
** File description:
** samsh
*/

#include "samsh.hpp"

samsh::samsh() {
    setEnv();
    run();
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
    return 0;
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
    std::string path = findPath(cmd);
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


