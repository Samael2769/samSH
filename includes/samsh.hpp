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

    protected:
    private:
        std::vector<std::string> _env;
        std::vector<std::string> _path;
        std::vector<std::string> _lastargs;
};

#endif /* !SAMSH_HPP_ */
