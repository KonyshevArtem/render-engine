#ifndef RENDER_ENGINE_FORWARD_RENDER_PASS_H
#define RENDER_ENGINE_FORWARD_RENDER_PASS_H

#include "render_pass.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_fence.h"

#include <memory>

class DrawRenderersPass;
class SkyboxPass;

class ForwardRenderPass : public RenderPass
{
public:
    explicit ForwardRenderPass(int priority);
    ~ForwardRenderPass() override = default;

    void Prepare(const GraphicsBackendRenderTargetDescriptor& colorTargetDescriptor, const GraphicsBackendRenderTargetDescriptor& depthTargetDescriptor);
    void Execute(const Context& ctx) override;
    const GraphicsBackendFence& GetEndFence() const;

    ForwardRenderPass(const ForwardRenderPass&) = delete;
    ForwardRenderPass(ForwardRenderPass&&) = delete;

    ForwardRenderPass &operator=(const ForwardRenderPass&) = delete;
    ForwardRenderPass &operator=(ForwardRenderPass&&) = delete;

private:
    std::shared_ptr<DrawRenderersPass> m_OpaquePass;
    std::shared_ptr<DrawRenderersPass> m_TransparentPass;
    std::shared_ptr<SkyboxPass> m_SkyboxPass;

    GraphicsBackendRenderTargetDescriptor m_ColorTargetDescriptor;
    GraphicsBackendRenderTargetDescriptor m_DepthTargetDescriptor;
    GraphicsBackendFence m_EndFence;
};

#endif //RENDER_ENGINE_FORWARD_RENDER_PASS_H
