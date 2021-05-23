#ifndef OPENGL_STUDY_MATRIX4X4_H
#define OPENGL_STUDY_MATRIX4X4_H

#include "../vector4/vector4.h"

typedef struct {
    float m00;
    float m01;
    float m02;
    float m03;
    float m10;
    float m11;
    float m12;
    float m13;
    float m20;
    float m21;
    float m22;
    float m23;
    float m30;
    float m31;
    float m32;
    float m33;
} matrix4x4;

matrix4x4 matrix4x4_zero();

matrix4x4 matrix4x4_identity();

matrix4x4 matrix4x4_translation(vector4 translation);

#endif //OPENGL_STUDY_MATRIX4X4_H
