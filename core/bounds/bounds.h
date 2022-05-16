#ifndef OPENGL_STUDY_BOUNDS
#define OPENGL_STUDY_BOUNDS

#include "vector3/vector3.h"
#include <array>
#include <span>
#include <vector>

struct Matrix4x4;

struct Bounds
{
    Vector3 Min;
    Vector3 Max;

    Bounds                 Combine(const Bounds &bounds) const;
    Vector3                GetCenter() const;
    Vector3                GetSize() const;
    Vector3                GetExtents() const;
    std::array<Vector3, 8> GetCornerPoints() const;

    static Bounds FromPoints(const std::span<Vector3> &_points);
    friend Bounds operator*(const Matrix4x4 &_matrix, const Bounds &_bounds);
};

#endif