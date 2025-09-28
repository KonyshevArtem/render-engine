#ifndef RENDER_ENGINE_FRUSTUM_H
#define RENDER_ENGINE_FRUSTUM_H

#include "matrix4x4/matrix4x4.h"
#include "vector4/vector4.h"
#include "bounds/bounds.h"

#include <cstdint>

struct Frustum
{
    enum Plane
    {
        NEAR,
        FAR,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,

        COUNT
    };

    static constexpr uint32_t AllPlanesBits = 1 << NEAR | 1 << FAR | 1 << LEFT | 1 << RIGHT | 1 << TOP | 1 << BOTTOM;
    static constexpr uint32_t SidePlanesBits = 1 << LEFT | 1 << RIGHT | 1 << TOP | 1 << BOTTOM;

    Vector4 Planes[Plane::COUNT];

    Frustum() = default;
    explicit Frustum(const Matrix4x4& viewProjectionMatrix);

    bool IsVisible(const Bounds& bounds, uint32_t planesBits = AllPlanesBits) const;
};

#endif //RENDER_ENGINE_FRUSTUM_H
