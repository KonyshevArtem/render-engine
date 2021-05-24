#include "matrix4x4.h"

matrix4x4 matrix4x4_zero() {
    matrix4x4 zero;
    zero.m00 = 0;
    zero.m01 = 0;
    zero.m02 = 0;
    zero.m03 = 0;
    zero.m10 = 0;
    zero.m11 = 0;
    zero.m12 = 0;
    zero.m13 = 0;
    zero.m20 = 0;
    zero.m21 = 0;
    zero.m22 = 0;
    zero.m23 = 0;
    zero.m30 = 0;
    zero.m31 = 0;
    zero.m32 = 0;
    zero.m33 = 0;
    return zero;
}

matrix4x4 matrix4x4_identity() {
    matrix4x4 identity = matrix4x4_zero();
    identity.m00 = 1;
    identity.m11 = 1;
    identity.m22 = 1;
    identity.m33 = 1;
    return identity;
}

matrix4x4 matrix4x4_translation(const vector4 *translation) {
    matrix4x4 translationMatrix = matrix4x4_identity();
    translationMatrix.m30 = translation->x;
    translationMatrix.m31 = translation->y;
    translationMatrix.m32 = translation->z;
    return translationMatrix;
}

float matrix4x4_getElement(const matrix4x4 *matrix, int column, int row) {
    float *floatPtr = (float *) matrix;
    return *(floatPtr + 4 * column + row);
}

void matrix4x4_setElement(const matrix4x4 *matrix, int column, int row, float value) {
    float *floatPtr = (float *) matrix;
    *(floatPtr + 4 * column + row) = value;
}

matrix4x4 matrix4x4_multiply(const matrix4x4 *a, const matrix4x4 *b) {
    matrix4x4 result = matrix4x4_zero();
    // TODO
    return result;
}
