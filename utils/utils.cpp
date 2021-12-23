#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mach-o/dyld.h>
#include <regex>
#include <string>

namespace Utils
{
    string ReadFile(const filesystem::path &_relativePath)
    {
        auto *file = fopen((GetExecutableDirectory() / _relativePath).c_str(), "r");
        if (file == nullptr)
            return "";

        if (fseek(file, 0, SEEK_END) != 0)
            return "";

        auto fileSize = ftell(file);
        if (fileSize == -1L)
            return "";

        if (fseek(file, 0, SEEK_SET) != 0)
            return "";

        string content(fileSize, ' ');
        int    c;
        long   i = -1;

        while ((c = fgetc(file)) != EOF)
            content[++i] = static_cast<char>(c);

        content[fileSize] = 0;

        fclose(file);

        return content;
    }

    string ReadFileWithIncludes(const filesystem::path &_relativePath) // NOLINT(misc-no-recursion)
    {
        auto file = ReadFile(_relativePath);

        regex  expression(R"(\s*#include\s+\"(.*)\"\s*\n)");
        smatch match;
        while (regex_search(file.cbegin(), file.cend(), match, expression))
        {
            auto includedFile = ReadFileWithIncludes(_relativePath.parent_path() / match[1].str());
            auto matchStart   = match.position();
            matchStart        = matchStart == 0 ? 0 : matchStart + 1;
            file              = file.replace(matchStart, match.length() - 2, includedFile);
        }

        return file;
    }

    void WriteFile(const filesystem::path &_relativePath, const string &_content)
    {
        std::ofstream o;
        o.open(GetExecutableDirectory() / _relativePath, std::ios::trunc);
        o << _content;
        o.close();
    }

    const filesystem::path &GetExecutableDirectory()
    {
        static filesystem::path executableDir;

        if (!executableDir.empty())
            return executableDir;

        char     path[100];
        uint32_t size = 100;

        if (_NSGetExecutablePath(&path[0], &size) != 0)
        {
            char longPath[size];
            _NSGetExecutablePath(&longPath[0], &size);
            executableDir = filesystem::path(longPath).parent_path();
        }
        else
            executableDir = filesystem::path(path).parent_path();

        return executableDir;
    }
} // namespace Utils