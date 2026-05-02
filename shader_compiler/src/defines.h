#ifndef RENDER_ENGINE_SHADER_COMPILER_DEFINES_H
#define RENDER_ENGINE_SHADER_COMPILER_DEFINES_H

#include <string>
#include <vector>
#include <set>

#include "hash.h"

std::vector<std::wstring> ConvertDefines(const std::vector<std::string>& defines)
{
    std::vector<std::wstring> wideDefines;
    for (const std::string& d : defines)
    {
        std::wstring define(d.c_str(), d.c_str() + d.size());
        wideDefines.emplace_back(std::move(define));
    }
    return wideDefines;
}

std::string GetDefinesHash(const std::vector<std::string>& defines)
{
    std::set<std::string> orderedDefines;
    for (const std::string& define : defines)
	    orderedDefines.insert(define);

    std::string combinedDefines;
    for (const std::string& define : orderedDefines)
	    combinedDefines += define;

    return std::to_string(Hash::FNV1a(combinedDefines));
}

std::string CombineDefines(const std::vector<std::string>& defines)
{
	std::string combinedDefines;
    if (defines.empty())
		combinedDefines = "<no defines>";
    else
    {
        for (const std::string& define : defines)
            combinedDefines += define + ",";
	}

    return combinedDefines;
}

#endif //RENDER_ENGINE_SHADER_COMPILER_DEFINES_H
