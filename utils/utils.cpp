#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#if __has_include("libloaderapi.h")
    #include <libloaderapi.h>
#elif __has_include("mach-o/dyld.h")
    #include <mach-o/dyld.h>
#endif
#include <regex>
#include <string>

namespace Utils
{
    std::string ReadFile(const std::filesystem::path &_relativePath)
    {
        auto  path = (GetExecutableDirectory() / _relativePath).string();
        auto *file = fopen(path.c_str(), "r");
        if (file == nullptr)
            throw std::runtime_error("[Utils] Can't open file: " + path);

        if (fseek(file, 0, SEEK_END) != 0)
            throw std::runtime_error("[Utils] Error reading file: " + path);

        auto fileSize = ftell(file);
        if (fileSize == -1L)
            throw std::runtime_error("[Utils] Error reading file: " + path);

        if (fseek(file, 0, SEEK_SET) != 0)
            throw std::runtime_error("[Utils] Error reading file: " + path);

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

    bool ReadFileBytes(const std::filesystem::path &_relativePath, std::vector<uint8_t> &bytes)
    {
        std::ifstream input(_relativePath.string(), std::ios::in | std::ios::binary);
        if (!input.is_open() || input.bad())
            return false;

        input.unsetf(std::ios::skipws);

        input.seekg(0, std::ios_base::end);
        size_t fileSize = input.tellg();
        input.seekg(0, std::ios_base::beg);

        bytes.reserve(fileSize);
        bytes.insert(bytes.begin(),
                     (std::istreambuf_iterator<char>(input)),
                     (std::istreambuf_iterator<char>()));

        input.close();

        return bytes.size() == fileSize;
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

#if RENDER_ENGINE_WINDOWS
		char path[MAX_PATH];

		GetModuleFileNameA(NULL, path, MAX_PATH);
		executableDir = std::filesystem::path(path).parent_path();
#elif RENDER_ENGINE_APPLE
        char     path[100];
        uint32_t size = 100;

        if (_NSGetExecutablePath(&path[0], &size) != 0)
        {
            char longPath[size];
            _NSGetExecutablePath(&longPath[0], &size);
            executableDir = std::filesystem::path(longPath).parent_path();
        }
        else
            executableDir = std::filesystem::path(path).parent_path();

        executableDir = executableDir.append("..").append("Resources");
#endif

        return executableDir;
    }

    size_t HashCombine(size_t hashA, size_t hashB)
    {
        // boost::hashCombine
        return hashA ^ (hashB + 0x9e3779b9 + (hashA << 6) + (hashA >> 2));
    }
} // namespace Utils