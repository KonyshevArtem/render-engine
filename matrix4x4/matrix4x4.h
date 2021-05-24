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

matrix4x4 matrix4x4_translation(const vector4 *translation);

float matrix4x4_getElement(const matrix4x4 *matrix, int column, int row);

void matrix4x4_setElement(const matrix4x4 *matrix, int column, int row, float value);

matrix4x4 matrix4x4_multiply(const matrix4x4 *a, const matrix4x4 *b);

#endif //OPENGL_STUDY_MATRIX4X4_H
