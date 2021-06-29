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

Vector3 Vector3::operator-() const
{
    return {-x, -y, -z};
}

Vector3 Vector3::operator+(const Vector3 &vector) const
{
    return {x + vector.x, y + vector.y, z + vector.z};
}

Vector3 Vector3::operator-(const Vector3 &vector) const
{
    return *this + (-vector);
}

Vector3 Vector3::operator*(float value) const
{
    return {value * x, value * y, value * z};
}

float Vector3::Dot(const Vector3 &a, const Vector3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(const Vector3 &a, const Vector3 &b)
{
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
