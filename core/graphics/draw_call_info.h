#ifndef RENDER_ENGINE_DRAW_CALL_INFO
#define RENDER_ENGINE_DRAW_CALL_INFO

#include "bounds/bounds.h"
#include "matrix4x4/matrix4x4.h"
#include "graphics_buffer/graphics_buffer_view.h"

#include <cstdint>
#include <vector>

class DrawableGeometry;
class Material;

struct DrawCallInfo
{
    const DrawableGeometry* Geometry = nullptr;
    const Material* Material = nullptr;
    std::vector<std::shared_ptr<GraphicsBufferView>> MatricesBufferViews;
    Bounds AABB{};
    bool CastShadows = false;
    bool Instanced = false;
    uint8_t StencilValue = 0;
    std::shared_ptr<GraphicsBufferView> InstancedMatricesEntriesView;
};

#endif