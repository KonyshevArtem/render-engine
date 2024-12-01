#if RENDER_ENGINE_EDITOR

#include "gizmos_pass.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"
#include "shader/shader.h"
#include "global_constants.h"
#include "graphics_backend_api.h"
#include "material/material.h"

#include <cmath>

GizmosPass::GizmosPass(int priority) :
    RenderPass(priority)
{
}

void GizmosPass::Execute(const Context& ctx)
{
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING"}, {}, {}, {});
    static const std::shared_ptr<Material> gizmosMaterial = std::make_shared<Material>(shader, "Gizmos");

    GraphicsBackend::Current()->BeginRenderPass("Gizmos pass");
    const auto& gizmos = Gizmos::GetGizmosToDraw();
    for (const auto& pair : gizmos)
    {
        const std::vector<Matrix4x4>& matrices = pair.second;
        auto begin = matrices.begin();
        int totalCount = matrices.size();
        while (totalCount > 0)
        {
            const auto end = begin + std::min(GlobalConstants::MaxInstancingCount, totalCount);
            const std::vector<Matrix4x4> matricesSlice(begin, end);
            Graphics::DrawInstanced(*pair.first, *gizmosMaterial, matricesSlice, 0);

            begin = end;
            totalCount -= GlobalConstants::MaxInstancingCount;
        }
    }
    GraphicsBackend::Current()->EndRenderPass();
}

#endif