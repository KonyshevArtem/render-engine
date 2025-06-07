#ifndef RENDER_ENGINE_SHADER_COMPILER_DEFINES_H
#define RENDER_ENGINE_SHADER_COMPILER_DEFINES_H

#include <iostream>
#include <vector>

#include "string_split.h"

std::vector<std::wstring> GetDefines(const std::string& commaSeparatedDefines)
{
    std::vector<std::string> defines = StringSplit::Split(commaSeparatedDefines, ',');

    std::vector<std::wstring> wideDefines;
    for (const std::string& d : defines)
    {
        std::wstring define(d.c_str(), d.c_str() + d.size());
        wideDefines.emplace_back(std::move(define));
    }
    return wideDefines;
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
