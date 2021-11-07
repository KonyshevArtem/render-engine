#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <mach-o/dyld.h>
#include <regex>
#include <string>

string Utils::ReadFile(const filesystem::path &_relativePath)
{
    FILE *file = fopen((GetExecutableDirectory() / _relativePath).c_str(), "r");
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
        content[i++] = static_cast<char>(c);

    content[fileSize] = 0;

    fclose(file);

    return content;
}

string Utils::ReadFileWithIncludes(const filesystem::path &_relativePath) // NOLINT(misc-no-recursion)
{
    string file = ReadFile(_relativePath);

    regex  expression(R"(\s*#include\s+\"(.*)\"\s*\n)");
    smatch match;
    while (regex_search(file.cbegin(), file.cend(), match, expression))
    {
        string includedFile = ReadFileWithIncludes(_relativePath.parent_path() / match[1].str());
        long   matchStart   = match.position();
        matchStart          = matchStart == 0 ? 0 : matchStart + 1;
        file                = file.replace(matchStart, match.length() - 2, includedFile);
    }

    return file;
}

filesystem::path Utils::GetExecutableDirectory()
{
    if (!m_ExecutableDir.empty())
        return m_ExecutableDir;

    char    *path = new char[10];
    uint32_t size = 10;

    if (_NSGetExecutablePath(&path[0], &size) != 0)
    {
        delete[] path;
        path = new char[size];
        _NSGetExecutablePath(path, &size);
    }

#pragma clang diagnostic push
#pragma ide diagnostic   ignored "DanglingPointer"
    string pathStr(path, size);
    delete[] path;
#pragma clang diagnostic pop

    m_ExecutableDir = filesystem::path(pathStr).parent_path();
    return m_ExecutableDir;
}
