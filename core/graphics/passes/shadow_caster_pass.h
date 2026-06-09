#ifndef RENDER_ENGINE_SHADOW_CASTER_PASS_H
#define RENDER_ENGINE_SHADOW_CASTER_PASS_H

#include "render_pass.h"
#include "graphics/data_structs/shadows_data.h"
#include "graphics/render_queue/render_queue.h"
#include "graphics/render_settings/render_settings.h"
#include "global_constants.h"
#include "light/light.h"
#include "graphics_buffer/ring_buffer.h"
#include "texture/texture_resources.h"

#include <memory>
#include <vector>
#include <string>

class GraphicsBuffer;
struct RenderData;
class Texture2DArray;
class Texture2D;
class Texture;
class Renderer;
class Light;
class Shader;

class ShadowCasterPass : public RenderPass
{
public:
    ShadowCasterPass();
    ~ShadowCasterPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

    void BindShadowMaps() const;

private:
    struct ShadowsCameraData
    {
        Matrix4x4 ViewMatrix;
        Matrix4x4 ProjectionMatrix;
        Vector4 LightPosOrDir;
        float FarPlane;
        uint32_t ShadowMapLayer;
    };

    std::shared_ptr<GraphicsBuffer> m_ShadowsConstantBuffer;
    TextureResources m_PunctualLightShadowAtlas;
    TextureResources m_DirectionLightShadowMap;

    RenderQueue m_DirectionalLightRenderQueues[GlobalConstants::ShadowCascadeCount];
    RenderQueue m_SpotLightRenderQueues[GlobalConstants::MaxSpotLightSources];
    RenderQueue m_PointLightsRenderQueues[GlobalConstants::MaxPointLightSources * 6];

    ShadowsData m_ShadowsGPUData{};
    ShadowsCameraData m_DirectionLightCameraData[GlobalConstants::ShadowCascadeCount];
    ShadowsCameraData m_SpotLightCameraData[GlobalConstants::MaxSpotLightSources];
    ShadowsCameraData m_PointLightCameraData[GlobalConstants::MaxPointLightSources * 6];

    std::shared_ptr<RingBuffer> m_ShadowCasterPassBuffer;
    std::vector<uint32_t> m_PunctualShadowAtlasSlots;

    Matrix4x4 m_BiasMatrix;
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Material> m_Material;

    void Render(RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, const ShadowsCameraData& cameraData, const std::string& passName) const;
    void PrepareCascade(uint32_t cascade, RenderData& renderData, const std::shared_ptr<GameObject>& lightGameObject);

    bool TryReserveShadowAtlasSlots(Light& light);
};

#endif //RENDER_ENGINE_SHADOW_CASTER_PASS_H
