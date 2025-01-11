#ifndef RENDER_ENGINE_UTILS_H
#define RENDER_ENGINE_UTILS_H

#include <cstdlib>
#include <string>

namespace Utils
{
    size_t HashFNV1a(const std::string& str);
    size_t HashCombine(size_t hashA, size_t hashB);
}; // namespace Utils

#endif //RENDER_ENGINE_UTILS_H