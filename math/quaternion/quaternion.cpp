#include "quaternion.h"
#include "../vector3/vector3.h"
#include <cmath>

Quaternion::Quaternion() :
    Quaternion(0, 0, 0, 1)
{
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) :
    x(_x), y(_y), z(_z), w(_w)
{
}

Quaternion Quaternion::AngleAxis(float _angleDeg, const Vector3 &_axis)
{
    auto normAxis     = _axis.Normalize();
    auto angleRadians = _angleDeg * static_cast<float>(M_PI) / 180;
    auto sin          = sinf(angleRadians / 2);
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

Quaternion Quaternion::operator*(const Quaternion &_quaternion) const
{
    return {
            x * _quaternion.w + y * _quaternion.z - z * _quaternion.y + w * _quaternion.x,
            -x * _quaternion.z + y * _quaternion.w + z * _quaternion.x + w * _quaternion.y,
            x * _quaternion.y - y * _quaternion.x + z * _quaternion.w + w * _quaternion.z,
            -x * _quaternion.x - y * _quaternion.y - z * _quaternion.z + w * _quaternion.w};
}

Vector3 Quaternion::operator*(const Vector3 &_vector) const
{
    auto u = Vector3(x, y, z);
    auto t = Vector3::Cross(u, _vector) * 2;
    return _vector + t * w + Vector3::Cross(u, t);
}
