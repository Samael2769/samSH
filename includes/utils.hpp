#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
char** vectorToCharPointerArray(const std::vector<std::string>& vec);
void freeCharPointerArray(char** arr);
bool isBinaryFile(const std::string& filename);