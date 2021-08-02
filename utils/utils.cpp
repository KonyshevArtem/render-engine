#include "utils.h"
#include "regex"
#include "string"
#include <cstdio>
#include <filesystem>

string Utils::ReadFile(const filesystem::path &path)
{
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr)
        return "";

    if (fseek(file, 0, SEEK_END) != 0)
        return "";

    long fileSize = ftell(file);
    if (fileSize == -1L)
        return "";

    if (fseek(file, 0, SEEK_SET) != 0)
        return "";

    string content(fileSize, ' ');
    int    c;
    long   i = 0;

    while ((c = fgetc(file)) != EOF)
        content[i++] = (char) c;

    content[fileSize] = 0;

    fclose(file);

    return content;
}

string Utils::ReadFileWithIncludes(const filesystem::path &path)
{
    string file = ReadFile(path);

    regex  expression("\\s*#include\\s+\\\"(.*)\\\"\\s*\n");
    smatch match;
    while (regex_search(file.cbegin(), file.cend(), match, expression))
    {
        string includedFile = ReadFileWithIncludes(path.parent_path() / match[1].str());
        file                = file.replace(match.position() + 1, match.length() - 2, includedFile);
    }

    return file;
}
