#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <sstream>
char** vectorToCharPointerArray(const std::vector<std::string>& vec);
void freeCharPointerArray(char** arr);
bool isBinaryFile(const std::string& filename);
std::vector<std::string> splitString(const std::string& str, char delimiter);
std::string joinString(const std::vector<std::string>& tokens, char delimiter, int start, int end);
std::string trim(const std::string& str);