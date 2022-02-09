#ifndef OPENGL_STUDY_BOUNDS
#define OPENGL_STUDY_BOUNDS

#include "../../math/vector3/vector3.h"
#include <vector>

struct Matrix4x4;

struct Bounds
{
    Vector3 Min;
    Vector3 Max;

    Bounds               Combine(const Bounds &bounds) const;
    Vector3              GetCenter() const;
    Vector3              GetSize() const;
    Vector3              GetExtents() const;
    std::vector<Vector3> GetCornerPoints() const;
    static Bounds        FromPoints(const std::vector<Vector3> &_points);

    friend Bounds operator*(const Matrix4x4 &_matrix, const Bounds &_bounds);
};

#endif