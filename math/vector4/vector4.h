#ifndef OPENGL_STUDY_VECTOR4_H
#define OPENGL_STUDY_VECTOR4_H

struct Vector4
{
public:
    float x;
    float y;
    float z;
    float w;

    Vector4();

    Vector4(float x, float y, float z, float w);

    static Vector4 Zero();

    float Length() const;

    Vector4 Normalize() const;
};

#endif//OPENGL_STUDY_VECTOR4_H
