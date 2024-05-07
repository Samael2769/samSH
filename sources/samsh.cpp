/*
** EPITECH PROJECT, 2022
** Untitled (Workspace)
** File description:
** samsh
*/

#include "samsh.hpp"
#include "script.hpp"

samsh::samsh() {
    setEnv();
    _builtins["exit"] = std::bind(&samsh::builtinExit, this, std::placeholders::_1);
    _builtins["cd"] = std::bind(&samsh::builtinCd, this, std::placeholders::_1);
    _builtins["env"] = std::bind(&samsh::builtinEnv, this, std::placeholders::_1);
    _builtins["setenv"] = std::bind(&samsh::builtinSetenv, this, std::placeholders::_1);
    _builtins["unsetenv"] = std::bind(&samsh::builtinUnsetenv, this, std::placeholders::_1);
    _builtins["getenv"] = std::bind(&samsh::builtinGetenv, this, std::placeholders::_1);
    _builtins["echo"] = std::bind(&samsh::builtinEcho, this, std::placeholders::_1);
    _status = 0;
    STDIN = dup(STDIN_FILENO);
    STDOUT = dup(STDOUT_FILENO);
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
        prompt p(_env);
        p.setStatus(_status);
        p.createPrompt("\e[0;36m[\\t]\e[0;m \e[0;32m\\u@\\h\e[0;m:\e[1;35m\\W\e[0;m->\e[1;32m\\$\e[0;m\n-> ");
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
    std::vector<std::string> tokens = splitString(input, ';'); // Split input string by semicolon delimiter
    pid_t pid;

    for (const auto& token : tokens) {
        _lastargs.clear();
        _lastargs = splitString(token, ' '); // Split input string by space delimiter
        if (_lastargs.empty()) {
            continue;
        }
        handlePipes();
    }
    return 0;
}

void printTable(const std::vector<std::vector<std::string>>& table) {
    for (const auto& row : table) {
        for (const auto& cell : row) {
            std::cout << cell << " $ ";
        }
        std::cout << std::endl;
    }
}

int samsh::exec(const std::string& cmd) {
    pid_t pid;
    int status;
    std::string path;

    if (scriptHandling() == 0) {
        return 0;
    }
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
        if (args[1] == "-") {
            if (getEnv("OLDPWD").empty()) {
                std::cerr << "cd: OLDPWD not set." << std::endl;
                return -1;
            }
            if (chdir(getEnv("OLDPWD").c_str()) == -1) {
                std::cerr << "cd: " << getEnv("OLDPWD") << ": No such file or directory." << std::endl;
                return -1;
            }
            std::string path = getcwd(nullptr, 0);
            setenv("OLDPWD", getEnv("PWD"), 1);
            setenv("PWD", path, 1);
        } else {
            setenv("OLDPWD", getEnv("PWD"), 1);
            if (chdir(args[1].c_str()) == -1) {
                std::cerr << "cd: " << args[1] << ": No such file or directory." << std::endl;
                return -1;
            }
            std::string path = getcwd(nullptr, 0);
            setenv("PWD", path, 1);
        }
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

int samsh::handleRedirection(std::vector<std::string> args)
{
    pid_t pid;
    int status;
    _lastargs = args;
    pid = fork();
    if (pid == 0) {
        choseRedirection();
        exec(args[0]);
        exit(_status);
    } else if (pid < 0) {
        std::cerr << "fork failed" << std::endl;
        return -1;
    } else {
        wait(&status);
        _status = WEXITSTATUS(status);
    }
    return 0;
}

int samsh::choseRedirection() {
    for (int i = 0; i < _lastargs.size(); i++) {
        if (_lastargs[i] == ">") {
            if (i + 1 < _lastargs.size()) {
                int fd = open(_lastargs[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    std::cerr << "Failed to open file" << std::endl;
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing name for redirect." << std::endl;
                return -1;
            }
        } else if (_lastargs[i] == ">>") {
            if (i + 1 < _lastargs.size()) {
                int fd = open(_lastargs[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1) {
                    std::cerr << "Failed to open file" << std::endl;
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing name for redirect." << std::endl;
                return -1;
            }
        } else if (_lastargs[i] == "<") {
            if (i + 1 < _lastargs.size()) {
                int fd = open(_lastargs[i + 1].c_str(), O_RDONLY);
                if (fd == -1) {
                    std::cerr << "Failed to open file" << std::endl;
                    return -1;
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing name for redirect." << std::endl;
                return -1;
            }
        } else if (_lastargs[i] == "2>") {
            if (i + 1 < _lastargs.size()) {
                int fd = open(_lastargs[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    std::cerr << "Failed to open file" << std::endl;
                    return -1;
                }
                if (dup2(fd, STDERR_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing name for redirect." << std::endl;
                return -1;
            }
        } else if (_lastargs[i] == "2>>") {
            if (i + 1 < _lastargs.size()) {
                int fd = open(_lastargs[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1) {
                    std::cerr << "Failed to open file" << std::endl;
                    return -1;
                }
                if (dup2(fd, STDERR_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing name for redirect." << std::endl;
                return -1;
            }
        } else if (_lastargs[i] == "<<") {
            if (i + 1 < _lastargs.size()) {
                std::string delimiter = _lastargs[i + 1];
                std::string line;
                std::string input;
                std::cout << ">";
                while (std::getline(std::cin, line)) {
                    if (line == delimiter) {
                        break;
                    }
                    std::cout << ">";
                    input += line + "\n";
                }
                int fd[2];
                if (pipe(fd) == -1) {
                    std::cerr << "Failed to create pipe" << std::endl;
                    return -1;
                }
                if (write(fd[1], input.c_str(), input.size()) == -1) {
                    std::cerr << "Failed to write to pipe" << std::endl;
                    return -1;
                }
                if (dup2(fd[0], STDIN_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
                close(fd[0]);
                close(fd[1]);
                _lastargs.erase(_lastargs.begin() + i, _lastargs.begin() + i + 2);
            } else {
                std::cerr << "Missing delimiter for here document." << std::endl;
                return -1;
            }
        }
    }
    return _status;
}

int samsh::handlePipes() {
    pid_t pid;
    int status;
    int fd[2];
    int prevfd = STDIN;
    std::string cmd = joinString(_lastargs, ' ', 0, _lastargs.size());
    std::vector<std::string> tokens = splitString(cmd, '|');

    for (int i = 0; i < tokens.size(); i++) {
        tokens[i] = trim(tokens[i]);
        std::vector<std::string> args = splitString(tokens[i], ' ');
        if (i < tokens.size() - 1) {
            if (pipe(fd) == -1) {
                std::cerr << "Failed to create pipe" << std::endl;
                return -1;
            }
        }
        pid = fork();
        if (pid == 0) {
            if (i < tokens.size() - 1) {
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
            }
            if (i > 0) {
                if (dup2(prevfd, STDIN_FILENO) == -1) {
                    std::cerr << "Failed to duplicate file descriptor" << std::endl;
                    return -1;
                }
            }
            if (i < tokens.size() - 1) {
                close(fd[0]);
                close(fd[1]);
            }
            handleRedirection(args);
            exit(_status);
        } else if (pid < 0) {
            std::cerr << "fork failed" << std::endl;
            return -1;
        } else {
            wait(&status);
            _status = WEXITSTATUS(status);
            if (i < tokens.size() - 1) {
                close(fd[1]);
                prevfd = fd[0];
            }
        }
    }
    return 0;
}

int samsh::scriptHandling() {
    std::string filepath = _lastargs[0];
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return -1;
    }
    
    std::string file_ctn;
    std::string line;
    
    for (int i = 0; std::getline(file, line); i++) {
        if (i == 0) {
            if (line.find("#!/samsh") == 0) {
                continue;
            } else
                return -1;
        }
        file_ctn += line + "\n";
    }
    
    file.close();
    
    if (file_ctn.empty()) {
        return -1;
    }
    
    script _script;
    _script.setScript(file_ctn);
    _script.run();
    
    return 0;
}
