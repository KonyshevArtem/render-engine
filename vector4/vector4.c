#include "vector4.h"

vector4 vector4_zero() {
    vector4 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    zero.w = 0;
    return zero;
}

vector4 vector4_build(float x, float y, float z, float w) {
    vector4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}
