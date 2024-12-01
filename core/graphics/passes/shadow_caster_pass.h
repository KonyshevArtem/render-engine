#ifndef RENDER_ENGINE_SHADOW_CASTER_PASS_H
#define RENDER_ENGINE_SHADOW_CASTER_PASS_H

#include "render_pass.h"
#include "graphics/data_structs/shadows_data.h"

#include <memory>
#include <vector>
#include <string>

class GraphicsBuffer;
struct Context;
class Texture2DArray;
class Texture2D;
class Renderer;

class ShadowCasterPass : public RenderPass
{
public:
    ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsConstantBuffer, int priority);
    ~ShadowCasterPass() override = default;

    void Prepare();
    void Execute(const Context &ctx) override;

    ShadowCasterPass(const ShadowCasterPass&) = delete;
    ShadowCasterPass(ShadowCasterPass&&) = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass&) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass&&) = delete;

private:
    std::shared_ptr<GraphicsBuffer> m_ShadowsConstantBuffer;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2D> m_DirectionLightShadowMap;
    std::shared_ptr<Texture2DArray> m_PointLightShadowMap;

    ShadowsData m_ShadowsData{};

    static void Render(const std::vector<std::shared_ptr<Renderer>>& renderers, const Vector4& viewport, const std::string& passName);
};

#endif //RENDER_ENGINE_SHADOW_CASTER_PASS_H
