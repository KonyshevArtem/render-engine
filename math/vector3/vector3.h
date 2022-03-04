#ifndef OPENGL_STUDY_VECTOR3_H
#define OPENGL_STUDY_VECTOR3_H

struct Vector4;

struct [[nodiscard]] Vector3
{
public:
    float x;
    float y;
    float z;

    Vector3();

    Vector3(float _x, float _y, float _z);

    Vector3(Vector4 _vector4);

    static const Vector3 &One();

    [[nodiscard]] float Length() const;

    Vector3 Normalize() const;

    Vector3 operator-() const;

    Vector3 operator+(const Vector3 &_vector) const;

    Vector3 operator-(const Vector3 &_vector) const;

    Vector3 operator*(float _value) const;

    Vector3 operator*(const Vector3 &_vector) const;

    Vector4 ToVector4(float _w) const;

    static float Dot(const Vector3 &_a, const Vector3 &_b);

    static Vector3 Cross(const Vector3 &_a, const Vector3 &_b);
};

#endif //OPENGL_STUDY_VECTOR3_H
