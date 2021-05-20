#include "matrix4x4.h"

matrix4x4 getZeroMatrix() {
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

matrix4x4 getIdentityMatrix() {
    matrix4x4 identity = getZeroMatrix();
    identity.m00 = 1;
    identity.m11 = 1;
    identity.m22 = 1;
    identity.m33 = 1;
    return identity;
}
