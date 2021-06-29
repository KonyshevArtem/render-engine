#ifndef OPENGL_STUDY_VECTOR3_H
#define OPENGL_STUDY_VECTOR3_H

struct Vector3
{
public:
    float x;
    float y;
    float z;

    Vector3();

    Vector3(float x, float y, float z);

    static Vector3 Zero();

    static Vector3 One();

    float Length() const;

    Vector3 Normalize() const;

    Vector3 operator-() const;

    Vector3 operator+(const Vector3 &vector) const;

    Vector3 operator-(const Vector3 &vector) const;

    Vector3 operator*(float value) const;

    static float Dot(const Vector3 &a, const Vector3 &b);

    static Vector3 Cross(const Vector3 &a, const Vector3 &b);
};

#endif//OPENGL_STUDY_VECTOR3_H
