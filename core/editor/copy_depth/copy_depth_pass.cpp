#include "copy_depth_pass.h"
#include "graphics_backend_api.h"
#include "graphics/graphics.h"
#include "texture_2d/texture_2d.h"
#include "types/graphics_backend_render_target_descriptor.h"

CopyDepthPass::CopyDepthPass(int priority) :
    RenderPass(priority),
    m_StartFence(),
    m_EndFence(GraphicsBackend::Current()->CreateFence(FenceType::COPY_TO_RENDER, "After Depth Copy"))
{
}

void CopyDepthPass::Prepare(const GraphicsBackendFence& waitForFence, const std::shared_ptr<Texture2D>& sourceDepth)
{
    m_StartFence = waitForFence;
    m_SourceDepth = sourceDepth;
}

void CopyDepthPass::Execute(const Context& ctx)
{
    GraphicsBackend::Current()->WaitForFence(m_StartFence);

    GraphicsBackend::Current()->BeginCopyPass("Copy Depth To Backbuffer");
    Graphics::CopyTextureToTexture(m_SourceDepth, nullptr, GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());
    GraphicsBackend::Current()->EndCopyPass();

    GraphicsBackend::Current()->SignalFence(m_EndFence);
    GraphicsBackend::Current()->WaitForFence(m_EndFence);
}
