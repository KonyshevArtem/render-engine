#ifndef OPENGL_STUDY_DRAW_CALL_INFO
#define OPENGL_STUDY_DRAW_CALL_INFO

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"
#include "vector3/vector3.h"
#include <functional>
#include <memory>

struct DrawCallInfo
{
    std::shared_ptr<DrawableGeometry> Geometry;
    std::shared_ptr<Material>         Material;
    Matrix4x4                         ModelMatrix;
    Bounds                            AABB;
    int                               InstancedIndex = -1;

    inline bool Instanced() const
    {
        return InstancedIndex >= 0;
    }

    enum class Sorting
    {
        FRONT_TO_BACK,
        BACK_TO_FRONT,
        NO_SORTING
    };

    struct Comparer
    {
        Sorting Sorting        = Sorting::FRONT_TO_BACK;
        Vector3 CameraPosition = Vector3();

        bool operator()(const DrawCallInfo &_i1, const DrawCallInfo &_i2);
    };

    struct Filter
    {
        std::function<bool(const DrawCallInfo &)> Delegate;

        static DrawCallInfo::Filter Opaque();
        static DrawCallInfo::Filter Transparent();
        static DrawCallInfo::Filter All();

        bool operator()(const DrawCallInfo &_info) const;
    };
};

#endif