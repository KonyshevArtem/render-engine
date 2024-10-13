#if RENDER_ENGINE_EDITOR

#include "gizmos_pass.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"
#include "shader/shader.h"
#include "global_constants.h"
#include "graphics_backend_api.h"

#include <cmath>

void GizmosPass::Execute(Context &_context)
{
    static auto gizmosMaterial = std::make_shared<Material>(Shader::Load("resources/shaders/gizmos", {"_INSTANCING"}, {}, {}, {}));

    GraphicsBackend::Current()->BeginRenderPass();
    {
        auto debugGroup = GraphicsBackendDebugGroup("Gizmos pass");

        const auto &gizmos = Gizmos::GetGizmosToDraw();
        for (const auto &pair : gizmos)
        {
            auto &matrices = pair.second;
            auto begin = matrices.begin();
            int totalCount = matrices.size();
            while (totalCount > 0)
            {
                std::vector<Matrix4x4> matricesSlice(begin, begin + std::min(GlobalConstants::MaxInstancingCount, totalCount));
                Graphics::DrawInstanced(*pair.first, *gizmosMaterial, matricesSlice, 0);

                begin += GlobalConstants::MaxInstancingCount;
                totalCount -= GlobalConstants::MaxInstancingCount;
            }
        }
    }

    GraphicsBackend::Current()->EndRenderPass();
}

#endif