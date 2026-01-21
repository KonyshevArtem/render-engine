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

    m_3DViewMatrix = ctx.ViewMatrix;
    m_3DProjectionMatrix = ctx.ProjectionMatrix;
    m_3DNearPlane = ctx.NearPlane;
    m_3DFarPlane = ctx.FarPlane;
    m_3DGizmosQueue.Prepare(m_3DProjectionMatrix * m_3DViewMatrix, Gizmos::Get3DGizmosToDraw(), RenderSettings{});

    m_2DProjectionMatrix = Matrix4x4::Orthographic(0, Graphics::GetScreenWidth(), 0, Graphics::GetScreenHeight(), 0.01f, 1);
    m_2DGizmosQueue.Prepare(m_2DProjectionMatrix, Gizmos::Get2DGizmosToDraw(), RenderSettings{.FrustumCullingPlanesBits = 0});

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

    Graphics::SetCameraData(m_3DViewMatrix, m_3DProjectionMatrix, m_3DNearPlane, m_3DFarPlane);
    m_3DGizmosQueue.Draw();

    Graphics::SetCameraData(Matrix4x4::Identity(), m_2DProjectionMatrix, 0.01f, 1.0f);
    m_2DGizmosQueue.Draw();

    GraphicsBackend::Current()->EndRenderPass();
}

#endif