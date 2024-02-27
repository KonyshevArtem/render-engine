#ifndef OPENGL_STUDY_DRAW_CALL_INFO
#define OPENGL_STUDY_DRAW_CALL_INFO

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"

struct DrawCallInfo
{
    DrawableGeometry *Geometry;
    Material *Material;
    Matrix4x4 ModelMatrix;
    Bounds AABB;
    bool CastShadows;
    bool Instanced;
    int InstancesDataIndex;
};

#endif