#include "utils.hpp"
#include <iostream>

char** vectorToCharPointerArray(const std::vector<std::string>& vec) {
    // Allocate memory for the array of char pointers
    char** arr = new char*[vec.size() + 1]; // +1 for the nullptr terminator
    for (size_t i = 0; i < vec.size(); ++i) {
        // Allocate memory for each string and copy its content
        arr[i] = new char[vec[i].size() + 1];
        std::strcpy(arr[i], vec[i].c_str());
    }
    // Null-terminate the array
    arr[vec.size()] = nullptr;
    return arr;
}

void freeCharPointerArray(char** arr) {
    for (size_t i = 0; arr[i] != nullptr; ++i) {
        delete[] arr[i];
    }
    delete[] arr;
}


bool isBinaryFile(const std::string& filename) {

    // Check if the file has execute permissions
    if (access(filename.c_str(), X_OK) != 0) {
        return false; // File does not have execute permissions
    }
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        // Unable to open file
        return false;
    }

    // Read the first 4 bytes (magic number) from the file
    char magic[4] = {0};
    file.read(magic, 4);

    // Check for ELF magic bytes
    if (magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
        return true; // ELF binary
    }

    // Check for Mach-O magic bytes
    // (Add similar check for Mach-O magic bytes if targeting macOS)

    return true;
}

// Split a string by a delimiter and return a vector of tokens
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string joinString(const std::vector<std::string>& tokens, char delimiter, int start, int end) {
    std::string str;
    for (int i = start; i < end; i++) {
        str += tokens[i];
        if (i != end - 1) {
            str += delimiter;
        }
    }
    return str;
}

std::string trim(const std::string& str) {
    std::string new_str = str;
    while (!new_str.empty() && std::isspace(new_str.front())) {
        new_str.erase(new_str.begin());
    }
    while (!new_str.empty() && std::isspace(new_str.back())) {
        new_str.pop_back();
    }
    return new_str;
}

std::vector<std::string> splitString(const std::string& str, std::string delimiters, bool keepDelimiters) {
    //if any of the delimiters is found, the string is split
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
        if (end != start) {
            tokens.push_back(str.substr(start, end - start));
        }
        if (keepDelimiters) {
            tokens.push_back(str.substr(end, 1));
        }
        start = end + 1;
    }
    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }
    for (int i = 0; i < tokens.size(); i++) {
        tokens[i] = trim(tokens[i]);
    }
    return tokens;
}

std::string removeChars(const std::string& str, const std::string& chars) {
    std::string new_str = str;
    for (int i = 0; i < chars.size(); i++) {
        for (int j = 0; j < new_str.size(); j++) {
            if (new_str[j] == chars[i]) {
                new_str.erase(new_str.begin() + j);
            }
        }
    }
    return new_str;
}