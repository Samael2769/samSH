#include "utils.hpp"


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

    return false; // Not a binary executable
}