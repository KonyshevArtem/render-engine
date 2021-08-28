#ifndef OPENGL_STUDY_VECTOR4_H
#define OPENGL_STUDY_VECTOR4_H

struct [[nodiscard]] Vector4
{
public:
    float x;
    float y;
    float z;
    float w;

    Vector4();

    Vector4(float _x, float _y, float _z, float _w);

    [[nodiscard]] float Length() const;

    Vector4 Normalize() const;
};

#endif //OPENGL_STUDY_VECTOR4_H
