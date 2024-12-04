#ifndef RENDER_ENGINE_SHADOW_CASTER_PASS_H
#define RENDER_ENGINE_SHADOW_CASTER_PASS_H

#include "render_pass.h"
#include "graphics/data_structs/shadows_data.h"
#include "graphics/render_queue/render_queue.h"
#include "global_constants.h"

#include <memory>
#include <vector>
#include <string>

class GraphicsBuffer;
struct Context;
class Texture2DArray;
class Texture2D;
class Texture;
class Renderer;
class Light;

class ShadowCasterPass : public RenderPass
{
public:
    ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsConstantBuffer, int priority);
    ~ShadowCasterPass() override = default;

    void Prepare(const std::vector<std::shared_ptr<Renderer>>& renderers, const std::vector<std::shared_ptr<Light>>& lights, float shadowsDistance);
    void Execute(const Context &ctx) override;

    ShadowCasterPass(const ShadowCasterPass&) = delete;
    ShadowCasterPass(ShadowCasterPass&&) = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass&) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass&&) = delete;

private:
    struct ShadowsMatrices
    {
        Matrix4x4 ViewMatrix;
        Matrix4x4 ProjectionMatrix;
    };

    std::shared_ptr<GraphicsBuffer> m_ShadowsConstantBuffer;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2D> m_DirectionLightShadowMap;
    std::shared_ptr<Texture2DArray> m_PointLightShadowMap;

    RenderQueue m_DirectionalLightRenderQueue;
    RenderQueue m_SpotLightRenderQueues[GlobalConstants::MaxSpotLightSources];
    RenderQueue m_PointLightsRenderQueues[GlobalConstants::MaxPointLightSources];

    ShadowsData m_ShadowsGPUData{};
    ShadowsMatrices m_DirectionLightMatrices;
    ShadowsMatrices m_SpotLightMatrices[GlobalConstants::MaxSpotLightSources];
    ShadowsMatrices m_PointLightMatrices[GlobalConstants::MaxPointLightSources * 6];

    static void Render(const RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, int targetLayer, const ShadowsMatrices& matrices, const std::string& passName);
};

#endif //RENDER_ENGINE_SHADOW_CASTER_PASS_H
