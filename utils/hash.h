#ifndef RENDER_ENGINE_HASH_H
#define RENDER_ENGINE_HASH_H

#include <cstdlib>
#include <string>

namespace Hash
{
    size_t FNV1a(const std::string& str);
    size_t Combine(size_t hashA, size_t hashB);
};

#endif //RENDER_ENGINE_HASH_H