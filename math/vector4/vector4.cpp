#include "vector4.h"
#include <cmath>

Vector4::Vector4(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vector4 Vector4::Zero()
{
    return {0, 0, 0, 0};
}

Vector4 Vector4::ToVector3() const
{
    return {this->x, this->y, this->z, 0};
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
