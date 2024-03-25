/*
** EPITECH PROJECT, 2022
** Untitled (Workspace)
** File description:
** samsh
*/

#ifndef SAMSH_HPP_
#define SAMSH_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include "utils.hpp"
#include <sstream>
#include <map>
#include <functional>
#include <fstream>
#include <fcntl.h>

class samsh {
    public:
        samsh();
        ~samsh();
        void setEnv();
        std::string getEnv(std::string var);
        void printEnv() { for (int i = 0; i < _env.size(); i++) std::cout << _env[i] << std::endl; }
        int run();
        int parse(std::string input);
        int exec(const std::string &cmd);
        void setPath();
        std::vector<std::string> splitString(const std::string& str, char delimiter);
        std::string findPath(const std::string& cmd);
        bool isBuiltin(const std::string& cmd, std::vector<std::string> args);
        int builtinExit(std::vector<std::string> args);
        int builtinCd(std::vector<std::string> args);
        int builtinEnv(std::vector<std::string> args);
        int builtinSetenv(std::vector<std::string> args);
        int builtinUnsetenv(std::vector<std::string> args);
        int builtinGetenv(std::vector<std::string> args);
        int builtinEcho(std::vector<std::string> args);
        int setenv(const std::string& name, const std::string& value, int overwrite);
        int unsetenv(const std::string& name);
        int handleRedirection();
    protected:
    private:
        std::vector<std::string> _env;
        std::vector<std::string> _path;
        std::vector<std::string> _lastargs;
        std::map<std::string, std::function<int(std::vector<std::string>)>> _builtins;
        int _status;
        int STDIN;
        int STDOUT;
};

#endif /* !SAMSH_HPP_ */
