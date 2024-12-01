#ifndef RENDER_ENGINE_GIZMOS_PASS_H
#define RENDER_ENGINE_GIZMOS_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"
#include "types/graphics_backend_fence.h"

#include <vector>
#include <memory>

struct Context;
class Renderer;

class GizmosPass : public RenderPass
{
public:
    explicit GizmosPass(int priority);
    ~GizmosPass() override = default;

    void Prepare(const std::vector<std::shared_ptr<Renderer>>& renderers, const GraphicsBackendFence& waitForFence);
    void Execute(const Context& ctx) override;

    GizmosPass(const GizmosPass&) = delete;
    GizmosPass(GizmosPass&&) = delete;

    GizmosPass &operator=(const GizmosPass&) = delete;
    GizmosPass &operator=(GizmosPass&&) = delete;

private:
    GraphicsBackendFence m_Fence;
};

#endif
#endif