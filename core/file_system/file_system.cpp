#include "file_system.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <set>
#include <regex>
#include <string>
#include <cassert>

#if RENDER_ENGINE_APPLE
#include <Foundation/NSBundle.hpp>
#elif RENDER_ENGINE_WINDOWS
#include <windows.h>
#endif

namespace FileSystem
{
    std::string ReadFile(const std::filesystem::path& path)
    {
        auto *file = fopen(path.string().c_str(), "r");
        if (file == nullptr)
            throw std::runtime_error("[Utils] Can't open file: " + path.string());

        if (fseek(file, 0, SEEK_END) != 0)
            throw std::runtime_error("[Utils] Error reading file: " + path.string());

        auto fileSize = ftell(file);
        if (fileSize == -1L)
            throw std::runtime_error("[Utils] Error reading file: " + path.string());

        if (fseek(file, 0, SEEK_SET) != 0)
            throw std::runtime_error("[Utils] Error reading file: " + path.string());

        std::string content(fileSize, ' ');
        int         c;
        long        i = -1;

        while ((c = fgetc(file)) != EOF)
            content[++i] = static_cast<char>(c);

        content[fileSize] = 0;

        fclose(file);

        return content;
    }

    std::string ReadFileWithIncludes_Internal(const std::filesystem::path& path, const std::regex& includeRegex, std::set<std::string>& includedFiles)
    {
        auto file = ReadFile(path);

        std::smatch match;
        while (std::regex_search(file.cbegin(), file.cend(), match, includeRegex))
        {
            auto matchStart = match.position();
            matchStart = matchStart == 0 ? 0 : matchStart + 1;

            std::filesystem::path pathToInclude = path.parent_path() / match[1].str();
            if (!includedFiles.contains(pathToInclude.string()))
            {
                includedFiles.insert(pathToInclude.string());

                auto includedFile = ReadFileWithIncludes_Internal(pathToInclude, includeRegex, includedFiles);
                includedFile = "\n" + includedFile + "\n";
                file = file.replace(matchStart, match.length() - 2, includedFile);
            }
            else
            {
                file = file.replace(matchStart, match.length() - 2, "");
            }
        }

        return file;
    }

    std::string ReadFileWithIncludes(const std::filesystem::path& path) // NOLINT(misc-no-recursion)
    {
        std::set<std::string> includedFiles;
        std::regex expression(R"(\s*#include\s+\"(.*)\"\s*\n)");

        return ReadFileWithIncludes_Internal(path, expression, includedFiles);
    }

    bool ReadFileBytes(const std::filesystem::path& relativePath, std::vector<uint8_t>& bytes)
    {
        std::ifstream input(relativePath.string(), std::ios::in | std::ios::binary);
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

    void WriteFile(const std::filesystem::path& path, const std::string& content)
    {
        std::ofstream o;
        o.open(path, std::ios::trunc);
        o << content;
        o.close();
    }

    const std::filesystem::path& GetExecutablePath()
    {
#if RENDER_ENGINE_APPLE
        static const std::filesystem::path path = std::filesystem::path(NS::Bundle::mainBundle()->executablePath()->cString(NS::UTF8StringEncoding));
#elif RENDER_ENGINE_WINDOWS
        static std::filesystem::path path;
        if (path.empty())
        {
            char executablePath[MAX_PATH];
            GetModuleFileNameA(NULL, executablePath, MAX_PATH);
            path = std::filesystem::path(executablePath);
        }
#endif
        return path;
    }

    const std::filesystem::path& GetResourcesPath()
    {
#if RENDER_ENGINE_APPLE
        static const std::filesystem::path path = std::filesystem::path(NS::Bundle::mainBundle()->resourcePath()->cString(NS::UTF8StringEncoding));
#elif RENDER_ENGINE_WINDOWS
        static std::filesystem::path path;
        if (path.empty())
        {
            char executablePath[MAX_PATH];
            GetModuleFileNameA(NULL, executablePath, MAX_PATH);
            path = std::filesystem::path(executablePath).parent_path();
        }
#endif
        return path;
    }

}