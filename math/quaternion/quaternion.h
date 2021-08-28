#ifndef OPENGL_STUDY_QUATERNION_H
#define OPENGL_STUDY_QUATERNION_H

#include "../vector3/vector3.h"

struct [[nodiscard]] Quaternion
{
public:
    float x;
    float y;
    float z;
    float w;

    Quaternion();

    Quaternion(float _x, float _y, float _z, float _w);

    static Quaternion AngleAxis(float _angleDeg, Vector3 _axis);

    Quaternion Inverse() const;

    Quaternion operator*(const Quaternion &_quaternion) const;

    Vector3 operator*(const Vector3 &_vector) const;
};

#endif //OPENGL_STUDY_QUATERNION_H
