#ifndef RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H
#define RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H

#include "graphics/passes/render_pass.h"

class DrawableGeometry;

class ShadowMapDebugPass : public RenderPass
{
public:
    explicit ShadowMapDebugPass(int priority);
    ~ShadowMapDebugPass() override = default;

    void Execute(const Context& ctx) override;

    ShadowMapDebugPass(const ShadowMapDebugPass&) = delete;
    ShadowMapDebugPass(ShadowMapDebugPass&&) = delete;

    ShadowMapDebugPass &operator=(const ShadowMapDebugPass&) = delete;
    ShadowMapDebugPass &operator=(ShadowMapDebugPass&&) = delete;

private:
    std::shared_ptr<DrawableGeometry> m_FullscreenMesh;
};

#endif //RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H
