#ifndef RENDER_ENGINE_DRAW_CALL_INFO
#define RENDER_ENGINE_DRAW_CALL_INFO

#include "bounds/bounds.h"
#include "matrix4x4/matrix4x4.h"

#include <cstdint>
#include <vector>

class DrawableGeometry;
class Material;

struct DrawCallInfo
{
    const DrawableGeometry* Geometry = nullptr;
    const Material* Material = nullptr;
    std::vector<Matrix4x4> ModelMatrices;
    Bounds AABB{};
    bool CastShadows = false;
    bool Instanced = false;
};

#endif