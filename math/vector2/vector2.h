#ifndef RENDER_ENGINE_VECTOR2_H
#define RENDER_ENGINE_VECTOR2_H

#include <cmath>

template<typename T>
struct [[nodiscard]] Vector2Base
{
public:
    T x;
    T y;

    Vector2Base() : x(0), y(0) {}
    Vector2Base(T _x, T _y) : x(_x), y(_y) {}

    static const Vector2Base& Zero()
    {
        static const Vector2Base zero = Vector2Base();
        return zero;
    }

    Vector2Base operator-() const
    {
        return { -this->x, -this->y };
    }

    Vector2Base operator+(const Vector2Base& _vector) const
    {
        return { this->x + _vector.x, this->y + _vector.y };
    }

    void operator+=(const Vector2Base& vector)
    {
        *this = *this + vector;
    }

    Vector2Base operator-(const Vector2Base&_vector) const
    {
        return { this->x - _vector.x, this->y - _vector.y };
    }

    void operator-=(const Vector2Base& vector)
    {
        *this = *this - vector;
    }

    Vector2Base operator*(T _value) const
    {
        return { this->x * _value, this->y * _value };
    }

    Vector2Base operator/(T value) const
    {
        return { x / value, y / value };
    }

    bool operator==(const Vector2Base& vector) const
    {
        return x == vector.x && y == vector.y;
    }

    bool operator!=(const Vector2Base& vector) const
    {
        return !(*this == vector);
    }

    T Length() const
    {
        return sqrt(x * x + y * y);
    }
};

using Vector2 = Vector2Base<float>;
using Vector2I = Vector2Base<int>;
using Vector2UI = Vector2Base<uint32_t>;

#endif //RENDER_ENGINE_VECTOR2_H
