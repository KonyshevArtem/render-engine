#include "vector4.h"
#include <cmath>
#include <limits>

const Vector4 &Vector4::Zero()
{
    static const Vector4 zero = Vector4();
    return zero;
}

Vector4::Vector4() :
    Vector4(0, 0, 0, 0)
{
}

Vector4::Vector4(float _x, float _y, float _z, float _w) :
    x(_x), y(_y), z(_z), w(_w)
{
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
    auto length = this->Length();
    if (length < std::numeric_limits<float>::epsilon())
        return {0, 0, 0, 0};

    return {
            this->x / length,
            this->y / length,
            this->z / length,
            this->w / length,
    };
}
