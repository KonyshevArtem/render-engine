#include "utils.h"
#include "string"
#include <cstdio>


char *Utils::ReadFile(const std::string &path)
{
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr)
        return nullptr;

    if (fseek(file, 0, SEEK_END) != 0)
        return nullptr;

    long fileSize = ftell(file);
    if (fileSize == -1L)
        return nullptr;

    if (fseek(file, 0, SEEK_SET) != 0)
        return nullptr;

    char *content = new char[fileSize + 1];
    int   c;
    long  i = 0;

    while ((c = fgetc(file)) != EOF)
        content[i++] = (char) c;

    content[fileSize] = 0;

    fclose(file);

    return content;
}
