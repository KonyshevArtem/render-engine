#include "frustum.h"

namespace FrustumLocal
{
    Vector4 NormalizeFrustumPlane(float a, float b, float c, float d)
    {
        Vector3 dir = {a, b, c};
        float length = dir.Length();
        return {a / length, b / length, c / length, d / length};
    }
}

Frustum::Frustum(const Matrix4x4& viewProjectionMatrix)
{
    Planes[NEAR] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 + viewProjectionMatrix.m02,
            viewProjectionMatrix.m13 + viewProjectionMatrix.m12,
            viewProjectionMatrix.m23 + viewProjectionMatrix.m22,
            viewProjectionMatrix.m33 + viewProjectionMatrix.m32
    );

    Planes[FAR] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 - viewProjectionMatrix.m02,
            viewProjectionMatrix.m13 - viewProjectionMatrix.m12,
            viewProjectionMatrix.m23 - viewProjectionMatrix.m22,
            viewProjectionMatrix.m33 - viewProjectionMatrix.m32
    );

    Planes[LEFT] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 + viewProjectionMatrix.m00,
            viewProjectionMatrix.m13 + viewProjectionMatrix.m10,
            viewProjectionMatrix.m23 + viewProjectionMatrix.m20,
            viewProjectionMatrix.m33 + viewProjectionMatrix.m30
    );

    Planes[RIGHT] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 - viewProjectionMatrix.m00,
            viewProjectionMatrix.m13 - viewProjectionMatrix.m10,
            viewProjectionMatrix.m23 - viewProjectionMatrix.m20,
            viewProjectionMatrix.m33 - viewProjectionMatrix.m30
    );

    Planes[TOP] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 - viewProjectionMatrix.m01,
            viewProjectionMatrix.m13 - viewProjectionMatrix.m11,
            viewProjectionMatrix.m23 - viewProjectionMatrix.m21,
            viewProjectionMatrix.m33 - viewProjectionMatrix.m31
    );

    Planes[BOTTOM] = FrustumLocal::NormalizeFrustumPlane(
            viewProjectionMatrix.m03 + viewProjectionMatrix.m01,
            viewProjectionMatrix.m13 + viewProjectionMatrix.m11,
            viewProjectionMatrix.m23 + viewProjectionMatrix.m21,
            viewProjectionMatrix.m33 + viewProjectionMatrix.m31
    );
}

bool Frustum::IsVisible(const Bounds& bounds, uint32_t planesBits) const
{
    for (int i = 0; i < Plane::COUNT; ++i)
    {
        if (((1 << i) & planesBits) == 0)
            continue;

        const Vector4& plane = Planes[i];

        Vector3 p = {
                plane.x >= 0 ? bounds.Max.x : bounds.Min.x,
                plane.y >= 0 ? bounds.Max.y : bounds.Min.y,
                plane.z >= 0 ? bounds.Max.z : bounds.Min.z
        };

        float distance = Vector3::Dot(plane, p) + plane.w;
        if (distance < 0)
            return false;
    }

    return true;
}