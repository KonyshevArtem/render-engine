#ifndef RENDER_ENGINE_SHADOW_CASTER_PASS_H
#define RENDER_ENGINE_SHADOW_CASTER_PASS_H

#include "graphics/data_structs/shadows_data.h"

#include <memory>
#include <vector>

class GraphicsBuffer;
struct Context;
class Texture2DArray;
class Texture2D;
class Renderer;

class ShadowCasterPass
{
public:
    explicit ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsUniformBlock);
    ~ShadowCasterPass() = default;

    void Execute(const Context &_ctx);

    ShadowCasterPass(const ShadowCasterPass &) = delete;
    ShadowCasterPass(ShadowCasterPass &&)      = delete;

    ShadowCasterPass &operator=(const ShadowCasterPass &) = delete;
    ShadowCasterPass &operator=(ShadowCasterPass &&) = delete;

private:
    static constexpr int SPOT_LIGHT_SHADOW_MAP_SIZE = 1024;
    static constexpr int DIR_LIGHT_SHADOW_MAP_SIZE  = 2048;
    static constexpr int POINT_LIGHT_SHADOW_MAP_FACE_SIZE = 512;

    std::shared_ptr<GraphicsBuffer> m_ShadowsUniformBlock;
    std::shared_ptr<Texture2DArray> m_SpotLightShadowMapArray;
    std::shared_ptr<Texture2D>      m_DirectionLightShadowMap;
    std::shared_ptr<Texture2DArray> m_PointLightShadowMap;

    ShadowsData m_ShadowsData{};

    static void Render(const std::vector<std::shared_ptr<Renderer>> &renderers, const Vector4& viewport);
};

#endif //RENDER_ENGINE_SHADOW_CASTER_PASS_H
