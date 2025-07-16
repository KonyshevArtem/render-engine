#include "vector3.h"
#include "vector4/vector4.h"
#include <cmath>
#include <limits>
#include <algorithm>

const Vector3 &Vector3::One()
{
    static const Vector3 one {1, 1, 1};
    return one;
}

Vector3::Vector3() :
    Vector3(0, 0, 0)
{
}

Vector3::Vector3(float _x, float _y, float _z) :
    x(_x), y(_y), z(_z)
{
}

Vector3::Vector3(Vector4 _vector4) :
    x(_vector4.x), y(_vector4.y), z(_vector4.z)
{
}

float Vector3::Length() const
{
    return sqrtf(this->x * this->x +
                 this->y * this->y +
                 this->z * this->z);
}

Vector3 Vector3::Normalize() const
{
    auto length = this->Length();
    if (length < std::numeric_limits<float>::epsilon())
        return {0, 0, 0};

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

Vector3 Vector3::operator+(const Vector3 &_vector) const
{
    return {x + _vector.x, y + _vector.y, z + _vector.z};
}

void Vector3::operator+=(const Vector3& vector)
{
    *this = *this + vector;
}

Vector3 Vector3::operator-(const Vector3 &_vector) const
{
    return *this + (-_vector);
}

void Vector3::operator-=(const Vector3& vector)
{
    *this = *this - vector;
}

Vector3 Vector3::operator*(float _value) const
{
    return {_value * x, _value * y, _value * z};
}

Vector3 Vector3::operator*(const Vector3 &_vector) const
{
    return {x * _vector.x, y * _vector.y, z * _vector.z};
}

Vector3 Vector3::operator/(const Vector3 &_vector) const
{
    return {x / _vector.x, y / _vector.y, z / _vector.z};
}

Vector4 Vector3::ToVector4(float _w) const
{
    return {x, y, z, _w};
}

float Vector3::Dot(const Vector3 &_a, const Vector3 &_b)
{
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
}

Vector3 Vector3::Cross(const Vector3 &_a, const Vector3 &_b)
{
    return {_a.y * _b.z - _a.z * _b.y, _a.z * _b.x - _a.x * _b.z, _a.x * _b.y - _a.y * _b.x};
}

Vector3 Vector3::Min(const Vector3& a, const Vector3& b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

Vector3 Vector3::Max(const Vector3& a, const Vector3& b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}
