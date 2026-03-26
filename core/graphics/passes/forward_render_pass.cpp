#include "forward_render_pass.h"
#include "draw_renderers_pass.h"
#include "skybox_pass.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "graphics/graphics.h"
#include "vector4/vector4.h"
#include "graphics/render_data.h"
#include "enums/resource_state.h"
#include "texture_2d/texture_2d.h"

ForwardRenderPass::ForwardRenderPass() :
    RenderPass(),
    m_EndFence(GraphicsBackend::Current()->CreateFence(FenceType::RENDER_TO_COPY, "After Forward Pass"))
{
    m_OpaquePass = std::make_unique<DrawRenderersPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque());
    m_TransparentPass = std::make_unique<DrawRenderersPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent());
    m_SkyboxPass = std::make_unique<SkyboxPass>();
}

ForwardRenderPass::~ForwardRenderPass()
{
    GraphicsBackend::Current()->DeleteFence(m_EndFence);
}

void ForwardRenderPass::Prepare(RenderData& renderData)
{
    Profiler::Marker marker("ForwardRenderPass::Prepare");

    const uint32_t width = renderData.Viewport.x;
    const uint32_t height = renderData.Viewport.y;
    if (m_CameraColorTarget == nullptr || m_CameraColorTarget->GetWidth() != width || m_CameraColorTarget->GetHeight() != height)
    {
        const TextureInternalFormat depthFormat = GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL ? TextureInternalFormat::DEPTH_32_STENCIL_8 : TextureInternalFormat::DEPTH_24_STENCIL_8;

        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = width;
        descriptor.Height = height;
        descriptor.Linear = true;
        descriptor.RenderTarget = true;

        descriptor.Format = TextureInternalFormat::RGBA16F;
        m_CameraColorTarget = Texture2D::Create(descriptor, "CameraColorRT");

        descriptor.Format = depthFormat;
        m_CameraDepthTarget = Texture2D::Create(descriptor, "CameraDepthRT");
    }

    renderData.CameraColorTarget = m_CameraColorTarget;
    renderData.CameraDepthTarget = m_CameraDepthTarget;
    
    m_OpaquePass->Prepare(renderData);
    m_SkyboxPass->Prepare(renderData);
    m_TransparentPass->Prepare(renderData);
}

void ForwardRenderPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("ForwardRenderPass::Execute");
    Profiler::GPUMarker gpuMarker("ForwardRenderPass::Execute");

    const GraphicsBackendRenderTargetDescriptor colorDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };
    const GraphicsBackendRenderTargetDescriptor depthDescriptor { .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

    GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(depthDescriptor);

    Graphics::SetCameraData(renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.NearPlane, renderData.FarPlane);

    GraphicsBackend::Current()->BeginRenderPass("Forward Render Pass");

    GraphicsBackend::Current()->SetViewport(0, 0, renderData.Viewport.x, renderData.Viewport.y, 0, 1);
    GraphicsBackend::Current()->SetScissorRect(0, 0, renderData.Viewport.x, renderData.Viewport.y);

	m_OpaquePass->Execute(renderData);
    m_SkyboxPass->Execute(renderData);
    m_TransparentPass->Execute(renderData);

    GraphicsBackend::Current()->EndRenderPass();
    GraphicsBackend::Current()->SignalFence(m_EndFence);
}

const GraphicsBackendFence& ForwardRenderPass::GetEndFence() const
{
    return m_EndFence;
}
