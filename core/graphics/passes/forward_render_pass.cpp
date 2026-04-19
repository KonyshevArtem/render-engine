#include "forward_render_pass.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "graphics/graphics.h"
#include "graphics/render_data.h"
#include "texture_2d/texture_2d.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics/render_settings/render_settings.h"

ForwardRenderPass::ForwardRenderPass() :
    RenderPass(),
    m_EndFence(GraphicsBackend::Current()->CreateFence(FenceType::RENDER_TO_COPY, "After Forward Pass"))
{
}

ForwardRenderPass::~ForwardRenderPass()
{
    GraphicsBackend::Current()->DeleteFence(m_EndFence);
}

void ForwardRenderPass::Prepare(RenderData& renderData)
{
    Profiler::Marker marker("ForwardRenderPass::Prepare");

	RenderSettings renderSettings{};
	renderSettings.Sorting = DrawCallSortMode::BACK_TO_FRONT;
	renderSettings.Filter = DrawCallFilter::Transparent();
	m_RenderQueue.Prepare(renderData.ProjectionMatrix * renderData.ViewMatrix, renderData.Renderers, renderSettings);
}

void ForwardRenderPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("ForwardRenderPass::Execute");

    const GraphicsBackendRenderTargetDescriptor colorDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
    const GraphicsBackendRenderTargetDescriptor depthDescriptor { .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };

    GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(depthDescriptor);

    Graphics::SetCameraData(renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.NearPlane, renderData.FarPlane);

    GraphicsBackend::Current()->BeginRenderPass("Forward Render Pass");
    {
        Profiler::GPUMarker gpuMarker("ForwardRenderPass::Execute");

        GraphicsBackend::Current()->SetViewport(0, 0, renderData.Viewport.x, renderData.Viewport.y, 0, 1);
        GraphicsBackend::Current()->SetScissorRect(0, 0, renderData.Viewport.x, renderData.Viewport.y);

		m_RenderQueue.Draw();
    }
    GraphicsBackend::Current()->EndRenderPass();
    GraphicsBackend::Current()->SignalFence(m_EndFence);
}

const GraphicsBackendFence& ForwardRenderPass::GetEndFence() const
{
    return m_EndFence;
}
