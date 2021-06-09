#include "quaternion.h"
#include <cmath>

Quaternion::Quaternion(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion Quaternion::AngleAxis(float angleDeg, Vector4 axis)
{
    Vector4 normAxis     = axis.ToVector3().Normalize();
    float   angleRadians = angleDeg * (float) M_PI / 180;
    float   sin          = sinf(angleRadians / 2);
    return {
            normAxis.x * sin,
            normAxis.y * sin,
            normAxis.z * sin,
            cosf(angleRadians / 2)};
}
