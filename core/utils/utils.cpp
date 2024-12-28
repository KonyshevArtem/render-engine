#include "utils.h"

namespace Utils
{
    size_t HashFNV1a(const std::string &str)
    {
        constexpr uint64_t fnvPrime = 1099511628211ULL;
        constexpr uint64_t fnvOffsetBasis = 14695981039346656037ULL;

        uint64_t hash = fnvOffsetBasis;

        for (const char c: str) {
            hash ^= c;
            hash *= fnvPrime;
        }

        return hash;
    }

    size_t HashCombine(size_t hashA, size_t hashB)
    {
        // boost::hashCombine
        return hashA ^ (hashB + 0x9e3779b9 + (hashA << 6) + (hashA >> 2));
    }
} // namespace Utils