#include "math_utils.h"

namespace Math
{
    float Lerp(float _a, float _b, float _t)
    {
        return (1 - _t) * _a + _t * _b;
    }
} // namespace Math
