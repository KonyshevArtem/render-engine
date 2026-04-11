#ifndef RENDER_ENGINE_VECTOR3_H
#define RENDER_ENGINE_VECTOR3_H

#include "vector4/vector4.h"

#include <cmath>
#include <limits>
#include <algorithm>

template<typename T>
struct [[nodiscard]] Vector3Base
{
public:
    T x;
    T y;
    T z;

    Vector3Base() : x(0), y(0), z(0) {}
    Vector3Base(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vector3Base(Vector4Base<T> _vector4) : x(_vector4.x), y(_vector4.y), z(_vector4.z) {}

    static const Vector3Base& Zero()
    {
        static const Vector3Base zero{ 0, 0, 0 };
        return zero;
    }

    static const Vector3Base& One()
    {
        static const Vector3Base one{ 1, 1, 1 };
        return one;
    }

    [[nodiscard]] T Length() const
    {
        return sqrt(this->x * this->x +
            this->y * this->y +
            this->z * this->z);
    }

    Vector3Base Normalize() const
    {
        const T length = Length();
        if (length < std::numeric_limits<T>::epsilon())
            return Zero();

        return {
                this->x / length,
                this->y / length,
                this->z / length,
        };
    }

    Vector3Base operator-() const
    {
        return { -x, -y, -z };
    }

    Vector3Base operator+(const Vector3Base& _vector) const
    {
        return { x + _vector.x, y + _vector.y, z + _vector.z };
    }

    void operator+=(const Vector3Base& vector)
    {
        *this = *this + vector;
    }

    Vector3Base operator-(const Vector3Base& _vector) const
    {
        return *this + (-_vector);
    }

    void operator-=(const Vector3Base& vector)
    {
        *this = *this - vector;
    }

    Vector3Base operator*(T _value) const
    {
        return { _value * x, _value * y, _value * z };
    }

    Vector3Base operator*(const Vector3Base& _vector) const
    {
        return { x * _vector.x, y * _vector.y, z * _vector.z };
    }

    Vector3Base operator/(const Vector3Base& _vector) const
    {
        return { x / _vector.x, y / _vector.y, z / _vector.z };
    }

    Vector4Base<T> ToVector4(T _w) const
    {
        return { x, y, z, _w };
    }

    static T Dot(const Vector3Base& _a, const Vector3Base& _b)
    {
        return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
    }

    static Vector3Base Cross(const Vector3Base& _a, const Vector3Base& _b)
    {
        return { _a.y * _b.z - _a.z * _b.y, _a.z * _b.x - _a.x * _b.z, _a.x * _b.y - _a.y * _b.x };
    }

    static Vector3Base Min(const Vector3Base& a, const Vector3Base& b)
    {
        return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
    }

    static Vector3Base Max(const Vector3Base& a, const Vector3Base& b)
    {
        return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
    }
};

using Vector3 = Vector3Base<float>;
using Vector3I = Vector3Base<int>;
using Vector3UI = Vector3Base<uint32_t>;

#endif //RENDER_ENGINE_VECTOR3_H
