#ifndef OPENGL_STUDY_VECTOR4_H
#define OPENGL_STUDY_VECTOR4_H

struct Vector4
{
public:
    float x;
    float y;
    float z;
    float w;

    Vector4(float x, float y, float z, float w);

    static Vector4 Zero();

    Vector4 ToVector3() const;

    float Length() const;

    Vector4 Normalize() const;
};

#endif//OPENGL_STUDY_VECTOR4_H
