#include "forward_render_pass.h"
#include "draw_renderers_pass.h"
#include "skybox_pass.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "graphics/graphics.h"
#include "vector4/vector4.h"
#include "graphics/context.h"
#include "enums/resource_state.h"

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

ForwardRenderPass::~ForwardRenderPass()
{
    GraphicsBackend::Current()->DeleteFence(m_EndFence);
}

void ForwardRenderPass::Prepare(const Context& ctx, const GraphicsBackendRenderTargetDescriptor& colorTargetDescriptor, const GraphicsBackendRenderTargetDescriptor& depthTargetDescriptor)
{
    Profiler::Marker marker("ForwardRenderPass::Prepare");

    m_ColorTargetDescriptor = colorTargetDescriptor;
    m_DepthTargetDescriptor = depthTargetDescriptor;

    const Matrix4x4 viewProj = ctx.ProjectionMatrix * ctx.ViewMatrix;
    m_OpaquePass->Prepare(viewProj, ctx.Renderers);
    m_SkyboxPass->Prepare();
    m_TransparentPass->Prepare(viewProj, ctx.Renderers);
}

void ForwardRenderPass::Execute(const Context& ctx)
{
    Profiler::Marker marker("ForwardRenderPass::Execute");
    Profiler::GPUMarker gpuMarker("ForwardRenderPass::Execute");

    GraphicsBackend::Current()->AttachRenderTarget(m_ColorTargetDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(m_DepthTargetDescriptor);

    Graphics::SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix, ctx.NearPlane, ctx.FarPlane);

    GraphicsBackend::Current()->BeginRenderPass("Forward Render Pass");

    GraphicsBackend::Current()->SetViewport(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight(), 0, 1);
    GraphicsBackend::Current()->SetScissorRect(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight());

    m_OpaquePass->Execute(ctx);
    m_SkyboxPass->Execute(ctx);
    m_TransparentPass->Execute(ctx);

    GraphicsBackend::Current()->TransitionRenderTarget(m_DepthTargetDescriptor, ResourceState::COMMON, GPUQueue::RENDER);
    GraphicsBackend::Current()->TransitionRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer(), ResourceState::COMMON, GPUQueue::RENDER);

    GraphicsBackend::Current()->EndRenderPass();
    GraphicsBackend::Current()->SignalFence(m_EndFence);
}

const GraphicsBackendFence& ForwardRenderPass::GetEndFence() const
{
    return m_EndFence;
}
