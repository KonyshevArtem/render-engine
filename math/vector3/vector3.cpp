#include "vector3.h"
#include <cmath>

Vector3::Vector3()
{
    x = y = z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3 Vector3::Zero()
{
    return {0, 0, 0};
}

Vector3 Vector3::One()
{
    return {1, 1, 1};
}

float Vector3::Length() const
{
    return sqrtf(this->x * this->x +
                 this->y * this->y +
                 this->z * this->z);
}

Vector3 Vector3::Normalize() const
{
    float length = this->Length();
    return {
            this->x / length,
            this->y / length,
            this->z / length,
    };
}
