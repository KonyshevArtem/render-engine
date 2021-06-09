#include "math_utils.h"

float Math::Lerp(float a, float b, float t)
{
    return (1 - t) * a + t * b;
}
