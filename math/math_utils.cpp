#include "math_utils.h"

namespace Math
{
    float Lerp(float _a, float _b, float _t)
    {
        return (1 - _t) * _a + _t * _b;
    }

    int Align(int x, int alignment)
    {
        if (alignment != 0 && x % alignment != 0)
            x = (x / alignment + 1) * alignment;
        return x;
    }
} // namespace Math
