#ifndef OPENGL_STUDY_QUATERNION_H
#define OPENGL_STUDY_QUATERNION_H

#include "../vector3/vector3.h"

struct Quaternion
{
public:
    float x;
    float y;
    float z;
    float w;

    Quaternion();

    Quaternion(float x, float y, float z, float w);

    static Quaternion AngleAxis(float angleDeg, Vector3 axis);
};

#endif//OPENGL_STUDY_QUATERNION_H
