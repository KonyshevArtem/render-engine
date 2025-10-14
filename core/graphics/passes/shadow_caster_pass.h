#ifndef RENDER_ENGINE_SHADOW_CASTER_PASS_H
#define RENDER_ENGINE_SHADOW_CASTER_PASS_H

#include "render_pass.h"
#include "graphics/data_structs/shadows_data.h"
#include "graphics/render_queue/render_queue.h"
#include "global_constants.h"
#include "light/light.h"
#include "graphics_buffer/ring_buffer.h"

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
    explicit ShadowCasterPass(int priority);
    ~ShadowCasterPass() override = default;

    void Prepare(const Context& ctx);
    void Execute(const Context& ctx) override;

    ShadowCasterPass(const ShadowCasterPass&) = delete;
    ShadowCasterPass(ShadowCasterPass&&) = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass&) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass&&) = delete;

private:
    struct ShadowsCameraData
    {
        Matrix4x4 ViewMatrix;
        Matrix4x4 ProjectionMatrix;
        Vector4 LightPosOrDir;
        float FarPlane;
    };

    std::shared_ptr<GraphicsBuffer> m_ShadowsConstantBuffer;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2DArray> m_DirectionLightShadowMap;
    std::shared_ptr<Texture2DArray> m_PointLightShadowMap;

    RenderQueue m_DirectionalLightRenderQueues[GlobalConstants::ShadowCascadeCount];
    RenderQueue m_SpotLightRenderQueues[GlobalConstants::MaxSpotLightSources];
    RenderQueue m_PointLightsRenderQueues[GlobalConstants::MaxPointLightSources * 6];

    ShadowsData m_ShadowsGPUData{};
    ShadowsCameraData m_DirectionLightCameraData[GlobalConstants::ShadowCascadeCount];
    ShadowsCameraData m_SpotLightCameraData[GlobalConstants::MaxSpotLightSources];
    ShadowsCameraData m_PointLightCameraData[GlobalConstants::MaxPointLightSources * 6];

    std::shared_ptr<RingBuffer> m_ShadowCasterPassBuffer;

    void Render(const RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, int targetLayer, const ShadowsCameraData &cameraData, const std::string& passName);
};

#endif //RENDER_ENGINE_SHADOW_CASTER_PASS_H
