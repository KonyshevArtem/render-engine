#ifndef RENDER_ENGINE_STRING_SPLIT_H
#define RENDER_ENGINE_STRING_SPLIT_H

#include <vector>
#include <string>

namespace StringSplit
{
    std::vector<std::string> Split(const std::string& string, char delimiter);
    std::vector<std::wstring> Split(const std::wstring& string, wchar_t delimiter);
}

#endif //RENDER_ENGINE_STRING_SPLIT_H
