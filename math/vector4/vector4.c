#include "vector4.h"
#include "math.h"

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

vector4 vector4_toVector3(const vector4 *vector) {
    vector4 result;
    result.x = vector->x;
    result.y = vector->y;
    result.z = vector->z;
    result.w = 0;
    return result;
}

float vector4_length(const vector4 *vector) {
    return sqrtf(vector->x * vector->x +
                 vector->y * vector->y +
                 vector->z * vector->z +
                 vector->w * vector->w);
}

vector4 vector4_normalize(const vector4 *vector) {
    float length = vector4_length(vector);
    vector4 result;
    result.x = vector->x / length;
    result.y = vector->y / length;
    result.z = vector->z / length;
    result.w = vector->w / length;
    return result;
}
