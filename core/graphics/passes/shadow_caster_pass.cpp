#include "shadow_caster_pass.h"
#include "graphics/graphics.h"
#include "graphics/context.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics/render_settings/render_settings.h"
#include "light/light.h"
#include "renderer/renderer.h"
#include "texture_2d/texture_2d.h"
#include "texture_2d_array/texture_2d_array.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug_group.h"
#include "enums/framebuffer_attachment.h"
#include "enums/load_action.h"
#include "material/material.h"
#include "shader/shader.h"
#include "types/graphics_backend_render_target_descriptor.h"

ShadowCasterPass::ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsConstantBuffer) :
    m_ShadowsConstantBuffer(std::move(shadowsConstantBuffer)),
    m_SpotLightShadowMapArray(Texture2DArray::ShadowMapArray(SPOT_LIGHT_SHADOW_MAP_SIZE, GlobalConstants::MaxSpotLightSources)),
    m_DirectionLightShadowMap(Texture2D::Create(DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE, TextureInternalFormat::DEPTH_COMPONENT, true, true)),
    m_PointLightShadowMap(Texture2DArray::ShadowMapArray(POINT_LIGHT_SHADOW_MAP_FACE_SIZE, GlobalConstants::MaxPointLightSources * 6))
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
    static const GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::DONT_CARE, .StoreAction = StoreAction::DONT_CARE };

    static const Matrix4x4 biasMatrix = Matrix4x4::TRS(Vector3{0.5f, 0.5f, 0.5f}, Quaternion(), Vector3{0.5f, 0.5f, 0.5f});

    static Matrix4x4 pointLightViewMatrices[6]{
            Matrix4x4::TBN({0, 0, 1}, {0, 1, 0}, {-1, 0, 0}).Invert(), // right
            Matrix4x4::TBN({0, 0, -1}, {0, 1, 0}, {1, 0, 0}).Invert(), // left
            Matrix4x4::TBN({1, 0, 0}, {0, 0, 1}, {0, -1, 0}).Invert(), // up
            Matrix4x4::TBN({1, 0, 0}, {0, 0, -1}, {0, 1, 0}).Invert(), // down
            Matrix4x4::TBN({1, 0, 0}, {0, 1, 0}, {0, 0, 1}).Invert(), // forward
            Matrix4x4::TBN({-1, 0, 0}, {0, 1, 0}, {0, 0, -1}).Invert(), // back
    };

    if (_ctx.ShadowCastersCount == 0)
        return;

    GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

    int spotLightsCount = 0;
    int pointLightsCount = 0;
    for (const auto *light: _ctx.Lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            depthTargetDescriptor.Layer = spotLightsCount;

            Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
            Graphics::SetRenderTarget(depthTargetDescriptor, m_SpotLightShadowMapArray);

            auto view = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            auto proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, _ctx.ShadowDistance);

            Graphics::SetCameraData(view, proj);
            m_ShadowsData.SpotLightsViewProjMatrices[spotLightsCount] = biasMatrix * proj * view;
            Render(_ctx.Renderers, {0, 0, SPOT_LIGHT_SHADOW_MAP_SIZE, SPOT_LIGHT_SHADOW_MAP_SIZE});

            ++spotLightsCount;
        }
        if (light->Type == LightType::POINT)
        {
            auto proj = Matrix4x4::Perspective(90, 1, 0.01f, _ctx.ShadowDistance);
            for (int i = 0; i < 6; ++i)
            {
                depthTargetDescriptor.Layer = pointLightsCount * 6 + i;

                Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
                Graphics::SetRenderTarget(depthTargetDescriptor, m_PointLightShadowMap);

                auto view = pointLightViewMatrices[i] * Matrix4x4::Translation(-light->Position);
                m_ShadowsData.PointLightShadows[pointLightsCount].ViewProjMatrices[i] = biasMatrix * proj * view;

                Graphics::SetCameraData(view, proj);
                Render(_ctx.Renderers, {0, 0, POINT_LIGHT_SHADOW_MAP_FACE_SIZE, POINT_LIGHT_SHADOW_MAP_FACE_SIZE});
            }

            m_ShadowsData.PointLightShadows[pointLightsCount].Position = light->Position.ToVector4(0);

            ++pointLightsCount;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            depthTargetDescriptor.Layer = 0;

            Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
            Graphics::SetRenderTarget(depthTargetDescriptor, m_DirectionLightShadowMap);

            auto &bounds = _ctx.ShadowCasterBounds;

            auto sizeWorldSpace   = bounds.GetSize();
            auto maxExtentWorldSpace = std::max({sizeWorldSpace.x, sizeWorldSpace.y, sizeWorldSpace.z});
            auto lightDir            = light->Rotation * Vector3 {0, 0, 1};
            auto viewPos             = bounds.GetCenter() - lightDir * maxExtentWorldSpace;
            auto viewMatrix          = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-viewPos);

            auto boundsViewSpace    = viewMatrix * bounds;
            auto extentsViewSpace   = boundsViewSpace.GetExtents();
            auto maxExtentViewSpace = std::max(extentsViewSpace.x, extentsViewSpace.y);
            auto projMatrix         = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0, _ctx.ShadowDistance);

            Graphics::SetCameraData(viewMatrix, projMatrix);

            m_ShadowsData.DirectionalLightViewProjMatrix = biasMatrix * projMatrix * viewMatrix;

            Render(_ctx.Renderers, {0, 0, DIR_LIGHT_SHADOW_MAP_SIZE, DIR_LIGHT_SHADOW_MAP_SIZE});
        }
    }

    m_ShadowsConstantBuffer->SetData(&m_ShadowsData, 0, sizeof(ShadowsData));
}

void ShadowCasterPass::Render(const std::vector<std::shared_ptr<Renderer>> &_renderers, const Vector4& viewport)
{
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("resources/shaders/shadowCaster", {}, {}, {}, {}));
    static RenderSettings renderSettings {DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), material};

    GraphicsBackend::Current()->BeginRenderPass();
    {
        auto debugGroup = GraphicsBackendDebugGroup("Render shadow map");

        Graphics::SetViewport(viewport);
        Graphics::DrawRenderers(_renderers, renderSettings);
    }
    GraphicsBackend::Current()->EndRenderPass();
}
