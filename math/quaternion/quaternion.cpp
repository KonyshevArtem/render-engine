#include "quaternion.h"
#include <cmath>

Quaternion::Quaternion()
{
    x = y = z = 0;
    w         = 1;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion Quaternion::AngleAxis(float angleDeg, Vector3 axis)
{
    Vector3 normAxis     = axis.Normalize();
    float   angleRadians = angleDeg * (float) M_PI / 180;
    float   sin          = sinf(angleRadians / 2);
    return {
            normAxis.x * sin,
            normAxis.y * sin,
            normAxis.z * sin,
            cosf(angleRadians / 2)};
}
