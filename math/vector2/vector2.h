#ifndef RENDER_ENGINE_VECTOR2_H
#define RENDER_ENGINE_VECTOR2_H

struct [[nodiscard]] Vector2
{
public:
    float x;
    float y;

    Vector2();

    Vector2(float _x, float _y);

    static const Vector2 &Zero();

    Vector2 operator-() const;

    Vector2 operator+(const Vector2 &_vector) const;

    Vector2 operator-(const Vector2 &_vector) const;

    Vector2 operator*(float _value) const;

    Vector2 operator/(float value) const;

    float Length() const;
};

#endif //RENDER_ENGINE_VECTOR2_H
