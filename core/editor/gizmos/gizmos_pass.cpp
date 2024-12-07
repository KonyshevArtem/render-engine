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
#include "renderer/renderer.h"
#include "quaternion/quaternion.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "editor/profiler/profiler.h"

#include <cmath>

GizmosPass::GizmosPass(int priority) :
    RenderPass(priority)
{
}

bool GizmosPass::Prepare(const std::vector<std::shared_ptr<Renderer>>& renderers, const GraphicsBackendFence& waitForFence)
{
    if (!Gizmos::IsEnabled())
        return false;

    Profiler::Marker marker("GizmosPass::Prepare");

    m_Fence = waitForFence;

    for (const std::shared_ptr<Renderer>& renderer : renderers)
    {
        if (renderer)
        {
            auto bounds = renderer->GetAABB();
            Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
        }
    }

    return true;
}

void GizmosPass::Execute(const Context& ctx)
{
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/gizmos", {"_INSTANCING"}, {}, {}, {});
    static const std::shared_ptr<Material> gizmosMaterial = std::make_shared<Material>(shader, "Gizmos");

    Profiler::Marker marker("GizmosPass::Execute");

    GraphicsBackend::Current()->WaitForFence(m_Fence);

    Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
    Graphics::SetRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

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

    Gizmos::ClearGizmos();
}

#endif