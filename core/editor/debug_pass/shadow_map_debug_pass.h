#ifndef RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H
#define RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H

#include "graphics/passes/render_pass.h"
#include "texture_2d/texture_2d.h"

class DrawableGeometry;

class ShadowMapDebugPass : public RenderPass
{
public:
    static bool DrawShadowMapOverlay;
    static bool DrawShadowCascades;

    explicit ShadowMapDebugPass(int priority);
    ~ShadowMapDebugPass() override = default;

    void Prepare(const std::shared_ptr<Texture2D>& depthMap);
    void Execute(const Context& ctx) override;

    ShadowMapDebugPass(const ShadowMapDebugPass&) = delete;
    ShadowMapDebugPass(ShadowMapDebugPass&&) = delete;

    ShadowMapDebugPass &operator=(const ShadowMapDebugPass&) = delete;
    ShadowMapDebugPass &operator=(ShadowMapDebugPass&&) = delete;

private:
    std::shared_ptr<DrawableGeometry> m_FullscreenMesh;
    std::shared_ptr<Texture2D> m_DepthMap;

    void DrawOverlay(const Context& ctx);
    void DrawCascades(const Context& ctx);
};

#endif //RENDER_ENGINE_SHADOW_MAP_DEBUG_PASS_H
