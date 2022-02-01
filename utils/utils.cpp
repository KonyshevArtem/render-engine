#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#ifdef OPENGL_STUDY_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#elif OPENGL_STUDY_MACOS
#include <mach-o/dyld.h>
#endif
#include <regex>
#include <string>

namespace Utils
{
    std::string ReadFile(const std::filesystem::path &_relativePath)
    {
        auto *file = fopen((GetExecutableDirectory() / _relativePath).string().c_str(), "r");
        if (file == nullptr)
            return "";

        if (fseek(file, 0, SEEK_END) != 0)
            return "";

        auto fileSize = ftell(file);
        if (fileSize == -1L)
            return "";

        if (fseek(file, 0, SEEK_SET) != 0)
            return "";

        std::string content(fileSize, ' ');
        int         c;
        long        i = -1;

        while ((c = fgetc(file)) != EOF)
            content[++i] = static_cast<char>(c);

        content[fileSize] = 0;

        fclose(file);

        return content;
    }

    std::string ReadFileWithIncludes(const std::filesystem::path &_relativePath) // NOLINT(misc-no-recursion)
    {
        auto file = ReadFile(_relativePath);

        std::regex  expression(R"(\s*#include\s+\"(.*)\"\s*\n)");
        std::smatch match;
        while (std::regex_search(file.cbegin(), file.cend(), match, expression))
        {
            auto includedFile = ReadFileWithIncludes(_relativePath.parent_path() / match[1].str());
            includedFile = "\n" + includedFile + "\n";
            auto matchStart   = match.position();
            matchStart        = matchStart == 0 ? 0 : matchStart + 1;
            file              = file.replace(matchStart, match.length() - 2, includedFile);
        }

        return file;
    }

    void WriteFile(const std::filesystem::path &_relativePath, const std::string &_content)
    {
        std::ofstream o;
        o.open(GetExecutableDirectory() / _relativePath, std::ios::trunc);
        o << _content;
        o.close();
    }

    const std::filesystem::path &GetExecutableDirectory()
    {
        static std::filesystem::path executableDir;

        if (!executableDir.empty())
            return executableDir;

#ifdef OPENGL_STUDY_WINDOWS
		char path[MAX_PATH];

		GetModuleFileNameA(NULL, path, MAX_PATH);
		executableDir = std::filesystem::path(path).parent_path();
#elif OPENGL_STUDY_MACOS
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
#endif

        return executableDir;
    }
} // namespace Utils