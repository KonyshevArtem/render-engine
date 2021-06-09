#ifndef OPENGL_STUDY_QUATERNION_H
#define OPENGL_STUDY_QUATERNION_H

#include "../vector4/vector4.h"

struct Quaternion
{
public:
    float x;
    float y;
    float z;
    float w;

    Quaternion(float x, float y, float z, float w);

    static Quaternion AngleAxis(float angleDeg, Vector4 axis);
};

#endif//OPENGL_STUDY_QUATERNION_H
