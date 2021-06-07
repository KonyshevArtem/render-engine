#include "quaternion.h"
#include "math.h"

quaternion quaternion_build(float x, float y, float z, float w)
{
    quaternion result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

quaternion quaternion_angle_axis(float angle_deg, const vector4 *axis)
{
    vector4 axisVector3   = vector4_toVector3(axis);
    vector4 normAxis      = vector4_normalize(&axisVector3);
    float   angle_radians = angle_deg * (float) M_PI / 180;
    float   sin           = sinf(angle_radians / 2);
    return quaternion_build(
            normAxis.x * sin,
            normAxis.y * sin,
            normAxis.z * sin,
            cosf(angle_radians / 2));
}
