#ifndef RENDER_ENGINE_GIZMOS_PASS_H
#define RENDER_ENGINE_GIZMOS_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"
#include "types/graphics_backend_fence.h"
#include "graphics/render_queue/render_queue.h"

#include <vector>
#include <memory>

struct Context;
class Renderer;

class GizmosPass : public RenderPass
{
public:
    explicit GizmosPass(int priority);
    ~GizmosPass() override = default;

    bool Prepare(const Context& ctx, const GraphicsBackendFence& waitForFence);
    void Execute(const Context& ctx) override;

    GizmosPass(const GizmosPass&) = delete;
    GizmosPass(GizmosPass&&) = delete;

    GizmosPass &operator=(const GizmosPass&) = delete;
    GizmosPass &operator=(GizmosPass&&) = delete;

private:
    GraphicsBackendFence m_Fence;
    RenderQueue m_GizmosQueue;
};

#endif
#endif