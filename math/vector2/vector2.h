#ifndef OPENGL_STUDY_VECTOR2_H
#define OPENGL_STUDY_VECTOR2_H

struct [[nodiscard]] Vector2
{
public:
    float x;
    float y;
    
    Vector2();

    Vector2(float _x, float _y);

    Vector2 operator-() const;

    Vector2 operator+(const Vector2 &_vector) const;

    Vector2 operator-(const Vector2 &_vector) const;

    Vector2 operator*(float _value) const;
};

#endif //OPENGL_STUDY_VECTOR2_H
