#ifndef OPENGL_STUDY_DRAW_CALL_INFO
#define OPENGL_STUDY_DRAW_CALL_INFO

#include "drawable_geometry/drawable_geometry.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"
#include <memory>

struct DrawCallInfo
{
    std::shared_ptr<DrawableGeometry> Geometry;
    std::shared_ptr<Material>         Material;
    Matrix4x4                         ModelMatrix;
};

#endif