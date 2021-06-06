#ifndef OPENGL_STUDY_QUATERNION_H
#define OPENGL_STUDY_QUATERNION_H

#include "../vector4/vector4.h"

typedef struct {
    float x;
    float y;
    float z;
    float w;
} quaternion;

quaternion quaternion_build(float x, float y, float z, float w);

quaternion quaternion_angle_axis(float angle_deg, const vector4 *axis);

#endif //OPENGL_STUDY_QUATERNION_H
