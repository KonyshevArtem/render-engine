#include "bounds.h"
#include "matrix4x4/matrix4x4.h"
#include "vector4/vector4.h"

Bounds Bounds::Combine(const Bounds &bounds) const
{
    return {
            {std::min(Min.x, bounds.Min.x), std::min(Min.y, bounds.Min.y), std::min(Min.z, bounds.Min.z)},
            {std::max(Max.x, bounds.Max.x), std::max(Max.y, bounds.Max.y), std::max(Max.z, bounds.Max.z)},
    };
}

Vector3 Bounds::GetCenter() const
{
    return (Min + Max) * 0.5f;
}

Vector3 Bounds::GetSize() const
{
    return Max - Min;
}

Vector3 Bounds::GetExtents() const
{
    return GetSize() * 0.5f;
}

std::array<Vector3, 8> Bounds::GetCornerPoints() const
{
    return std::array<Vector3, 8> {
            Min,
            {Min.x, Min.y, Max.z},
            {Min.x, Max.y, Min.z},
            {Min.x, Max.y, Max.z},
            {Max.x, Min.y, Min.z},
            {Max.x, Min.y, Max.z},
            {Max.x, Max.y, Min.z},
            Max,
    };
}

Bounds Bounds::FromPoints(const std::span<Vector3> &_points)
{
    if (_points.size() == 0)
        return Bounds();

    Bounds bounds;
    bounds.Min = _points[0];
    bounds.Max = _points[0];

    for (const auto &point: _points)
    {
        if (point.x > bounds.Max.x)
            bounds.Max.x = point.x;
        if (point.x < bounds.Min.x)
            bounds.Min.x = point.x;

        if (point.y > bounds.Max.y)
            bounds.Max.y = point.y;
        if (point.y < bounds.Min.y)
            bounds.Min.y = point.y;

        if (point.z > bounds.Max.z)
            bounds.Max.z = point.z;
        if (point.z < bounds.Min.z)
            bounds.Min.z = point.z;
    }
    return bounds;
}

Bounds operator*(const Matrix4x4 &_matrix, const Bounds &_bounds)
{
    auto corners = _bounds.GetCornerPoints();
    for (auto &point: corners)
        point = _matrix * point.ToVector4(1);

    return Bounds::FromPoints(std::span<Vector3>(corners.data(), corners.size()));
}
