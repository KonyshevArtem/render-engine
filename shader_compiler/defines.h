#ifndef RENDER_ENGINE_SHADER_COMPILER_DEFINES_H
#define RENDER_ENGINE_SHADER_COMPILER_DEFINES_H

#include <iostream>
#include <vector>

std::vector<std::wstring> GetDefines(int argc, char** argv)
{
    std::vector<std::wstring> defines;
    for (int i = 4; i < argc; ++i)
    {
        std::wstring define(argv[i], argv[i] + strlen(argv[i]));
        defines.emplace_back(std::move(define));
    }
    return defines;
}

std::string GetDefinesHash(std::vector<std::wstring> defines)
{
    auto HashFNV1a = [](const std::string &str)
    {
        constexpr uint64_t fnvPrime = 1099511628211ULL;
        constexpr uint64_t fnvOffsetBasis = 14695981039346656037ULL;

        uint64_t hash = fnvOffsetBasis;

        for (const char c: str) {
            hash ^= c;
            hash *= fnvPrime;
        }

        return hash;
    };

    std::sort(defines.begin(), defines.end());

    std::string combinedDefines;
    for (const auto& define : defines)
    {
        std::string d(define.begin(), define.end());
        combinedDefines += d + ",";
    }

    return std::to_string(HashFNV1a(combinedDefines));
}

void PrintDefines(const std::vector<std::wstring>& defines, const std::string& definesHash)
{
    std::cout << "Hash: " << definesHash << std::endl;

    if (defines.empty())
    {
        std::cout << "<No defines>";
    }
    else
    {
        for (auto& define: defines)
        {
            std::wcout << define << " ";
        }
    }
    std::cout << std::endl;
}

#endif //RENDER_ENGINE_SHADER_COMPILER_DEFINES_H
