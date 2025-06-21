#ifndef RENDER_ENGINE_ARGUMENTS_H
#define RENDER_ENGINE_ARGUMENTS_H

#include <string>

namespace Arguments
{
    void Init(char** argv, int argc);
    bool Contains(const std::string& argument);
    std::string Get(const std::string& argument);
}

#endif //RENDER_ENGINE_ARGUMENTS_H
