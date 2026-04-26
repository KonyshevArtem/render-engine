#ifndef RENDER_ENGINE_VECTOR4_H
#define RENDER_ENGINE_VECTOR4_H

#include <cmath>
#include <limits>

template<typename T>
struct [[nodiscard]] Vector4Base
{
public:
    T x;
    T y;
    T z;
    T w;

    Vector4Base() : x(0), y(0), z(0), w(0) {}
    Vector4Base(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

    [[nodiscard]] T Length() const
    {
        return sqrt(this->x * this->x +
            this->y * this->y +
            this->z * this->z +
            this->w * this->w);
    }

    Vector4Base Normalize() const
    {
        const T length = this->Length();
        if (length < std::numeric_limits<T>::epsilon())
            return Zero();

        return {
                this->x / length,
                this->y / length,
                this->z / length,
                this->w / length,
        };
    }

    static const Vector4Base& Zero()
    {
        static const Vector4Base zero = Vector4Base();
        return zero;
    }

    Vector4Base operator/(T a)
    {
        return Vector4Base(x / a, y / a, z / a, w / a);
    }

    void operator/=(T a)
    {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
    }

    T& operator[](int index)
    {
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        return w;
    }
};

using Vector4 = Vector4Base<float>;
using Vector4I = Vector4Base<int>;
using Vector4UI = Vector4Base<uint32_t>;

#endif //RENDER_ENGINE_VECTOR4_H
