#include "arguments.h"

#include <unordered_map>

namespace Arguments
{
    std::unordered_map<std::string, std::string> s_Arguments;

    void Init(char** argv, int argc)
    {
        char* prevArgument = nullptr;
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                s_Arguments[argv[i]] = "";
                prevArgument = argv[i];
            }
            else if (prevArgument)
                s_Arguments[prevArgument] = argv[i];
        }
    }

    bool Contains(const std::string& argument)
    {
        return s_Arguments.contains(argument);
    }

    std::string Get(const std::string& argument)
    {
        return s_Arguments.contains(argument) ? s_Arguments.at(argument) : "";
    }
}