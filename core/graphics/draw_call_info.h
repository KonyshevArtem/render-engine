#ifndef RENDER_ENGINE_DRAW_CALL_INFO
#define RENDER_ENGINE_DRAW_CALL_INFO

#include "bounds/bounds.h"
#include "matrix4x4/matrix4x4.h"

#include <cstdint>

class DrawableGeometry;
class Material;

struct DrawCallInfo
{
    DrawableGeometry *Geometry;
    Material *Material;
    Matrix4x4 ModelMatrix;
    Bounds AABB;
    uint32_t PerInstanceDataIndex;
    uint32_t PerInstanceDataOffset;
    bool CastShadows;
    bool Instanced;
    size_t InstancesDataIndex;
};

#endif