#if RENDER_ENGINE_EDITOR

#include "gizmos_pass.h"
#include "gizmos.h"
#include "graphics/render_data.h"
#include "graphics/graphics.h"
#include "graphics_backend_api.h"
#include "renderer/renderer.h"
#include "quaternion/quaternion.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "editor/profiler/profiler.h"
#include "graphics/render_settings/render_settings.h"
#include "texture/texture.h"

#include <cmath>

GizmosPass::GizmosPass(int priority, Mode mode) :
    RenderPass(priority),
	m_Mode(mode)
{
}

void GizmosPass::Prepare(RenderData& renderData)
{
    if (m_Mode == Mode::GIZMOS_3D && !Gizmos::IsEnabled3D() ||
        m_Mode == Mode::GIZMOS_2D && !Gizmos::IsEnabled2D())
        return;

    Profiler::Marker marker("GizmosPass::Prepare");

    if (m_Mode == Mode::GIZMOS_3D)
    {
        for (const std::shared_ptr<Renderer>& renderer : renderData.Renderers)
        {
            if (renderer)
            {
                auto bounds = renderer->GetAABB();
                Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
            }
        }

        m_ViewMatrix = renderData.ViewMatrix;
        m_ProjectionMatrix = renderData.ProjectionMatrix;
        m_NearPlane = renderData.NearPlane;
        m_FarPlane = renderData.FarPlane;
        m_GizmosQueue.Prepare(m_ProjectionMatrix * m_ViewMatrix, Gizmos::Get3DGizmosToDraw(), RenderSettings{});
    }
    else
    {
        m_NearPlane = 0.01f;
        m_FarPlane = 1.0f;
        m_ViewMatrix = Matrix4x4::Identity();
        m_ProjectionMatrix = Matrix4x4::Orthographic(0, renderData.Viewport.x, 0, renderData.Viewport.y, m_NearPlane, m_FarPlane);
        m_GizmosQueue.Prepare(m_ProjectionMatrix, Gizmos::Get2DGizmosToDraw(), RenderSettings{ .FrustumCullingPlanesBits = 0 });
    }
}

void GizmosPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("GizmosPass::Execute");
    Profiler::GPUMarker gpuMarker("GizmosPass::Execute");

    if (m_GizmosQueue.IsEmpty())
        return;

    const std::shared_ptr<Texture> colorTarget = m_Mode == Mode::GIZMOS_3D ? renderData.CameraColorTarget : renderData.PostProcessedTarget;
    const std::shared_ptr<Texture> depthTarget = m_Mode == Mode::GIZMOS_3D ? renderData.CameraDepthTarget : nullptr;

    const GraphicsBackendRenderTargetDescriptor colorDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = colorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
    GraphicsBackendRenderTargetDescriptor depthDescriptor = GraphicsBackendRenderTargetDescriptor::EmptyDepth();
    if (depthTarget)
		depthDescriptor = { .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = depthTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };

    GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(depthDescriptor);

    GraphicsBackend::Current()->BeginRenderPass("Gizmos pass");

    Graphics::SetCameraData(m_ViewMatrix, m_ProjectionMatrix, m_NearPlane, m_FarPlane);
    m_GizmosQueue.Draw();

    GraphicsBackend::Current()->EndRenderPass();
}

#endif