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

Quaternion Quaternion::Inverse() const
{
    return {-x, -y, -z, w};
}

Quaternion Quaternion::operator*(const Quaternion &quaternion) const
{
    return {
            x * quaternion.w + y * quaternion.z - z * quaternion.y + w * quaternion.x,
            -x * quaternion.z + y * quaternion.w + z * quaternion.x + w * quaternion.y,
            x * quaternion.y - y * quaternion.x + z * quaternion.w + w * quaternion.z,
            -x * quaternion.x - y * quaternion.y - z * quaternion.z + w * quaternion.w};
}

Vector3 Quaternion::operator*(const Vector3 &vector) const
{
    Vector3 u = Vector3(x, y, z);
    Vector3 t = Vector3::Cross(u, vector) * 2;
    return vector + t * w + Vector3::Cross(u, t);
}
