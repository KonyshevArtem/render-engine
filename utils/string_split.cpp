#include "string_split.h"
#include <sstream>

namespace StringSplit
{
    std::vector<std::string> Split(const std::string& string, char delimiter)
    {
        std::vector<std::string> result;

        std::string word;
        std::stringstream stream(string);
        while (std::getline(stream, word, delimiter))
            result.emplace_back(word);

        return result;
    }

    std::vector<std::wstring> Split(const std::wstring& string, wchar_t delimiter)
    {
        std::vector<std::wstring> result;

        std::wstring word;
        std::wstringstream stream(string);
        while (std::getline(stream, word, delimiter))
            result.emplace_back(word);

        return result;
    }
}
