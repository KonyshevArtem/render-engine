#include "vector4.h"
#include <cmath>

Vector4::Vector4() :
    Vector4(0, 0, 0, 0)
{
}

Vector4::Vector4(float _x, float _y, float _z, float _w)
{
    this->x = _x;
    this->y = _y;
    this->z = _z;
    this->w = _w;
}

float Vector4::Length() const
{
    return sqrtf(this->x * this->x +
                 this->y * this->y +
                 this->z * this->z +
                 this->w * this->w);
}

Vector4 Vector4::Normalize() const
{
    float length = this->Length();
    return {
            this->x / length,
            this->y / length,
            this->z / length,
            this->w / length,
    };
}
