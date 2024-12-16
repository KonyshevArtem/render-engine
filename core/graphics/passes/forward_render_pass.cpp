#include "forward_render_pass.h"
#include "draw_renderers_pass.h"
#include "skybox_pass.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "graphics/graphics.h"
#include "vector4/vector4.h"
#include "graphics/context.h"

ForwardRenderPass::ForwardRenderPass(int priority) :
    RenderPass(priority),
    m_ColorTargetDescriptor(),
    m_DepthTargetDescriptor(),
    m_EndFence(GraphicsBackend::Current()->CreateFence(FenceType::RENDER_TO_COPY, "After Forward Pass"))
{
    m_OpaquePass = std::make_unique<DrawRenderersPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque(), 1);
    m_TransparentPass = std::make_unique<DrawRenderersPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent(), 3);
    m_SkyboxPass = std::make_unique<SkyboxPass>(2);
}

void ForwardRenderPass::Prepare(const GraphicsBackendRenderTargetDescriptor& colorTargetDescriptor, const GraphicsBackendRenderTargetDescriptor& depthTargetDescriptor, const Vector3& cameraPosition, const std::vector<std::shared_ptr<Renderer>>& renderers)
{
    Profiler::Marker marker("ForwardRenderPass::Prepare");

    m_ColorTargetDescriptor = colorTargetDescriptor;
    m_DepthTargetDescriptor = depthTargetDescriptor;

    m_OpaquePass->Prepare(cameraPosition, renderers);
    m_SkyboxPass->Prepare();
    m_TransparentPass->Prepare(cameraPosition, renderers);
}

void ForwardRenderPass::Execute(const Context& ctx)
{
    Profiler::Marker marker("ForwardRenderPass::Execute");
    Profiler::GPUMarker gpuMarker("ForwardRenderPass:Execute");

    GraphicsBackend::Current()->AttachRenderTarget(m_ColorTargetDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(m_DepthTargetDescriptor);

    Graphics::SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

    GraphicsBackend::Current()->BeginRenderPass("Forward Render Pass");
    Graphics::SetViewport({0, 0, static_cast<float>(Graphics::GetScreenWidth()), static_cast<float>(Graphics::GetScreenHeight())});

    m_OpaquePass->Execute(ctx);
    m_SkyboxPass->Execute(ctx);
    m_TransparentPass->Execute(ctx);

    GraphicsBackend::Current()->EndRenderPass();
    GraphicsBackend::Current()->SignalFence(m_EndFence);
}

const GraphicsBackendFence& ForwardRenderPass::GetEndFence() const
{
    return m_EndFence;
}
