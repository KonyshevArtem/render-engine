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
};

#endif//OPENGL_STUDY_VECTOR3_H
