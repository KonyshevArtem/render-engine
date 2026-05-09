#ifndef RENDER_ENGINE_ARGUMENTS_H
#define RENDER_ENGINE_ARGUMENTS_H

#include <charconv>
#include <string>

namespace Arguments
{
    void Init(char** argv, int argc);
    bool Contains(const std::string& argument);
    std::string Get(const std::string& argument);

    template<typename T>
    T Get(const std::string& argument)
	{
        const std::string strVal = Get(argument);
        if (strVal.empty())
            return T();

        T value;
        auto [ptr, ec] = std::from_chars(strVal.data(), strVal.data() + strVal.size(), value);
        if (ec != std::errc{})
	        return T();

        return value;
    }
}

#endif //RENDER_ENGINE_ARGUMENTS_H
