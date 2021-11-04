#include "quaternion.h"
#include "../vector3/vector3.h"
#include <cmath>

Quaternion::Quaternion() :
    Quaternion(0, 0, 0, 1)
{
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
    this->x = _x;
    this->y = _y;
    this->z = _z;
    this->w = _w;
}

Quaternion Quaternion::AngleAxis(float _angleDeg, Vector3 _axis)
{
    Vector3 normAxis     = _axis.Normalize();
    float   angleRadians = _angleDeg * static_cast<float>(M_PI) / 180;
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
    Vector3 u = Vector3(x, y, z);
    Vector3 t = Vector3::Cross(u, _vector) * 2;
    return _vector + t * w + Vector3::Cross(u, t);
}
