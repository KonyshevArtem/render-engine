#if RENDER_ENGINE_EDITOR

#include "gizmos_pass.h"
#include "gizmos.h"
#include "graphics/context.h"
#include "graphics/graphics.h"
#include "graphics_backend_debug_group.h"
#include "shader/shader.h"
#include "graphics_backend_api.h"
#include "material/material.h"
#include "renderer/renderer.h"
#include "quaternion/quaternion.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "editor/profiler/profiler.h"
#include "graphics/render_settings/render_settings.h"

#include <cmath>

GizmosPass::GizmosPass(int priority) :
    RenderPass(priority)
{
}

bool GizmosPass::Prepare(const Context& ctx, const GraphicsBackendFence& waitForFence)
{
    if (!Gizmos::IsEnabled())
        return false;

    Profiler::Marker marker("GizmosPass::Prepare");

    m_Fence = waitForFence;

    for (const std::shared_ptr<Renderer>& renderer : ctx.Renderers)
    {
        if (renderer)
        {
            auto bounds = renderer->GetAABB();
            Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
        }
    }

    const Matrix4x4 vpMatrix = ctx.ProjectionMatrix * ctx.ViewMatrix;
    m_GizmosQueue.Prepare(vpMatrix, Gizmos::GetGizmosToDraw(), RenderSettings{});

    Gizmos::ClearGizmos();

    return true;
}

void GizmosPass::Execute(const Context& ctx)
{
    Profiler::Marker marker("GizmosPass::Execute");
    Profiler::GPUMarker gpuMarker("GizmosPass::Execute");

    GraphicsBackend::Current()->WaitForFence(m_Fence);

    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

    GraphicsBackend::Current()->BeginRenderPass("Gizmos pass");
    Graphics::DrawRenderQueue(m_GizmosQueue);
    GraphicsBackend::Current()->EndRenderPass();
}

#endif