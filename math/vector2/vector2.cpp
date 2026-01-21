#include "vector2.h"
#include <cmath>

const Vector2 &Vector2::Zero()
{
    static const Vector2 zero = Vector2();
    return zero;
}

Vector2::Vector2() :
    Vector2(0, 0)
{
}

Vector2::Vector2(float _x, float _y) :
    x(_x), y(_y)
{
}

Vector2 Vector2::operator-() const
{
    return {-this->x, -this->y};
}

Vector2 Vector2::operator+(const Vector2 &_vector) const
{
    return {this->x + _vector.x, this->y + _vector.y};
}

void Vector2::operator+=(const Vector2& vector)
{
    *this = *this + vector;
}

Vector2 Vector2::operator-(const Vector2 &_vector) const
{
    return {this->x - _vector.x, this->y - _vector.y};
}

void Vector2::operator-=(const Vector2& vector)
{
    *this = *this - vector;
}

Vector2 Vector2::operator*(float _value) const
{
    return {this->x * _value, this->y * _value};
}

Vector2 Vector2::operator/(float value) const
{
    return {x / value, y / value};
}

bool Vector2::operator==(const Vector2& vector) const
{
    return x == vector.x && y == vector.y;
}

bool Vector2::operator!=(const Vector2& vector) const
{
    return !(*this == vector);
}

float Vector2::Length() const
{
    return sqrtf(x * x + y * y);
}
