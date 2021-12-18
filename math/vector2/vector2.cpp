#include "vector2.h"

const Vector2 Vector2::m_Zero = Vector2();

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

Vector2 Vector2::operator-(const Vector2 &_vector) const
{
    return {this->x - _vector.x, this->y - _vector.y};
}

Vector2 Vector2::operator*(float _value) const
{
    return {this->x * _value, this->y * _value};
}
