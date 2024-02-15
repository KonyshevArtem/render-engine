#include "shadow_caster_pass.h"
#include "graphics/context.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics/render_settings.h"
#include "light/light.h"
#include "renderer/renderer.h"
#include "texture_2d/texture_2d.h"
#include "texture_2d_array/texture_2d_array.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug.h"
#include "enums/clear_mask.h"

#include <utility>

ShadowCasterPass::ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsUniformBlock) :
    m_ShadowsUniformBlock(std::move(shadowsUniformBlock)),
    m_SpotLightShadowMapArray(Texture2DArray::ShadowMapArray(SPOT_LIGHT_SHADOW_MAP_SIZE, Graphics::MAX_SPOT_LIGHT_SOURCES)),
    m_DirectionLightShadowMap(Texture2D::CreateShadowMap(DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE)),
    m_PointLightShadowMap(Texture2DArray::ShadowMapArray(POINT_LIGHT_SHADOW_MAP_FACE_SIZE, Graphics::MAX_POINT_LIGHT_SOURCES * 6))
{
    Graphics::SetGlobalTexture("_DirLightShadowMap", m_DirectionLightShadowMap);
    Graphics::SetGlobalTexture("_SpotLightShadowMapArray", m_SpotLightShadowMapArray);
    Graphics::SetGlobalTexture("_PointLightShadowMapArray", m_PointLightShadowMap);

    m_DirectionLightShadowMap->SetBorderColor({1, 1, 1, 1});
    m_DirectionLightShadowMap->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);

    m_SpotLightShadowMapArray->SetBorderColor({1, 1, 1, 1});
    m_SpotLightShadowMapArray->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);

    m_PointLightShadowMap->SetBorderColor({1, 1, 1, 1});
    m_PointLightShadowMap->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
}

void ShadowCasterPass::Execute(const Context &_ctx)
{
    static const Matrix4x4 biasMatrix = Matrix4x4::TRS(Vector3{0.5f, 0.5f, 0.5f}, Quaternion(), Vector3{0.5f, 0.5f, 0.5f});

    static Matrix4x4 pointLightViewMatrices[6]{
            Matrix4x4::TBN({0, 0, 1}, {0, 1, 0}, {-1, 0, 0}).Invert(), // right
            Matrix4x4::TBN({0, 0, -1}, {0, 1, 0}, {1, 0, 0}).Invert(), // left
            Matrix4x4::TBN({1, 0, 0}, {0, 0, 1}, {0, -1, 0}).Invert(), // up
            Matrix4x4::TBN({1, 0, 0}, {0, 0, -1}, {0, 1, 0}).Invert(), // down
            Matrix4x4::TBN({1, 0, 0}, {0, 1, 0}, {0, 0, 1}).Invert(), // forward
            Matrix4x4::TBN({-1, 0, 0}, {0, 1, 0}, {0, 0, -1}).Invert(), // back
    };

    if (_ctx.ShadowCasters.empty())
        return;

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Shadow pass");

    int spotLightsCount = 0;
    int pointLightsCount = 0;
    for (const auto *light: _ctx.Lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            Graphics::SetViewport({0, 0, SPOT_LIGHT_SHADOW_MAP_SIZE, SPOT_LIGHT_SHADOW_MAP_SIZE});
            Graphics::SetRenderTargets(nullptr, 0, 0, m_SpotLightShadowMapArray, 0, spotLightsCount);

            auto view = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            auto proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, _ctx.ShadowDistance);

            Graphics::SetCameraData(view, proj);
            m_ShadowsData.SpotLightsViewProjMatrices[spotLightsCount] = biasMatrix * proj * view;
            Render(_ctx.ShadowCasters);

            ++spotLightsCount;
        }
        if (light->Type == LightType::POINT)
        {
            Graphics::SetViewport({0, 0, POINT_LIGHT_SHADOW_MAP_FACE_SIZE, POINT_LIGHT_SHADOW_MAP_FACE_SIZE});

            auto proj = Matrix4x4::Perspective(90, 1, 0.01f, _ctx.ShadowDistance);
            for (int i = 0; i < 6; ++i)
            {
                Graphics::SetRenderTargets(nullptr, 0, 0, m_PointLightShadowMap, 0, pointLightsCount * 6 + i);

                auto view = pointLightViewMatrices[i] * Matrix4x4::Translation(-light->Position);
                m_ShadowsData.PointLightShadows[pointLightsCount].ViewProjMatrices[i] = biasMatrix * proj * view;

                Graphics::SetCameraData(view, proj);
                Render(_ctx.ShadowCasters);
            }

            m_ShadowsData.PointLightShadows[pointLightsCount].Position = light->Position.ToVector4(0);

            ++pointLightsCount;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            Graphics::SetViewport({0, 0, DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE});
            Graphics::SetRenderTargets(nullptr, 0, 0, m_DirectionLightShadowMap, 0, 0);

            auto bounds = _ctx.ShadowCasters[0]->GetAABB();
            for (const auto &renderer: _ctx.ShadowCasters)
                bounds = bounds.Combine(renderer->GetAABB());

            auto extentsWorldSpace   = bounds.GetExtents();
            auto maxExtentWorldSpace = std::max({extentsWorldSpace.x, extentsWorldSpace.y, extentsWorldSpace.z});
            auto lightDir            = light->Rotation * Vector3 {0, 0, 1};
            auto viewPos             = bounds.GetCenter() - lightDir * maxExtentWorldSpace;
            auto viewMatrix          = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-viewPos);

            auto boundsViewSpace    = viewMatrix * bounds;
            auto extentsViewSpace   = boundsViewSpace.GetExtents();
            auto maxExtentViewSpace = std::max(extentsViewSpace.x, extentsViewSpace.y);
            auto projMatrix         = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0, _ctx.ShadowDistance);

            Graphics::SetCameraData(viewMatrix, projMatrix);

            m_ShadowsData.DirectionalLightViewProjMatrix = biasMatrix * projMatrix * viewMatrix;

            Render(_ctx.ShadowCasters);
        }
    }

    m_ShadowsUniformBlock->SetData(&m_ShadowsData, 0, sizeof(ShadowsData));

    Graphics::SetRenderTargets(nullptr, 0, 0, nullptr, 0, 0);
}

void ShadowCasterPass::Render(const std::vector<Renderer *> &_renderers)
{
    static RenderSettings renderSettings {{{"LightMode", "ShadowCaster"}}};

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Render shadow map");

    GraphicsBackend::SetDepthWrite(true);
    GraphicsBackend::Clear(ClearMask::COLOR_DEPTH);

    auto drawCalls = Graphics::DoCulling(_renderers);
    Graphics::Draw(drawCalls, renderSettings);
}
