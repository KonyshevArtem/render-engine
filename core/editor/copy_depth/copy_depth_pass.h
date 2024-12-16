#ifndef RENDER_ENGINE_COPY_DEPTH_PASS_H
#define RENDER_ENGINE_COPY_DEPTH_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"
#include "types/graphics_backend_fence.h"

#include <memory>

class Texture2D;

class CopyDepthPass : public RenderPass
{
public:
    explicit CopyDepthPass(int priority);
    ~CopyDepthPass() override;

    void Prepare(const GraphicsBackendFence& waitForFence, const std::shared_ptr<Texture2D>& sourceDepth);
    void Execute(const Context& ctx) override;
    const GraphicsBackendFence& GetEndFence() const;

    CopyDepthPass(const CopyDepthPass&) = delete;
    CopyDepthPass(CopyDepthPass&&) = delete;

    CopyDepthPass &operator=(const CopyDepthPass&) = delete;
    CopyDepthPass &operator=(CopyDepthPass&&) = delete;

private:
    std::shared_ptr<Texture2D> m_SourceDepth;
    GraphicsBackendFence m_StartFence;
    GraphicsBackendFence m_EndFence;
};

#endif
#endif //RENDER_ENGINE_COPY_DEPTH_PASS_H
