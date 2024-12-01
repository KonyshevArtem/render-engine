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

constexpr int k_SpotLightShadowMapSize = 1024;
constexpr int k_DirLightShadowMapSize = 2048;
constexpr int k_PointLightShadowMapSize = 512;

ShadowCasterPass::ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsConstantBuffer, int priority) :
    RenderPass(priority),
    m_ShadowsConstantBuffer(std::move(shadowsConstantBuffer)),
    m_SpotLightShadowMapArray(Texture2DArray::ShadowMapArray(k_SpotLightShadowMapSize, GlobalConstants::MaxSpotLightSources, "SpotLightShadowMap")),
    m_DirectionLightShadowMap(Texture2D::Create(k_DirLightShadowMapSize, k_DirLightShadowMapSize, TextureInternalFormat::DEPTH_32, true, true, "DirectionalShadowMap")),
    m_PointLightShadowMap(Texture2DArray::ShadowMapArray(k_PointLightShadowMapSize, GlobalConstants::MaxPointLightSources * 6, "PointLightShadowMap"))
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

void ShadowCasterPass::Prepare()
{
}

void ShadowCasterPass::Execute(const Context& ctx)
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

    if (ctx.ShadowCastersCount == 0)
        return;

    GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

    int spotLightsCount = 0;
    int pointLightsCount = 0;
    for (const auto *light: ctx.Lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            depthTargetDescriptor.Layer = spotLightsCount;

            Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
            Graphics::SetRenderTarget(depthTargetDescriptor, m_SpotLightShadowMapArray);

            auto view = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            auto proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, ctx.ShadowDistance);

            Graphics::SetCameraData(view, proj);
            m_ShadowsData.SpotLightsViewProjMatrices[spotLightsCount] = biasMatrix * proj * view;
            Render(ctx.Renderers, {0, 0, k_SpotLightShadowMapSize, k_SpotLightShadowMapSize}, "Spot Light Shadow Pass " + std::to_string(spotLightsCount));

            ++spotLightsCount;
        }
        if (light->Type == LightType::POINT)
        {
            auto proj = Matrix4x4::Perspective(90, 1, 0.01f, ctx.ShadowDistance);
            for (int i = 0; i < 6; ++i)
            {
                depthTargetDescriptor.Layer = pointLightsCount * 6 + i;

                Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
                Graphics::SetRenderTarget(depthTargetDescriptor, m_PointLightShadowMap);

                auto view = pointLightViewMatrices[i] * Matrix4x4::Translation(-light->Position);
                m_ShadowsData.PointLightShadows[pointLightsCount].ViewProjMatrices[i] = biasMatrix * proj * view;

                Graphics::SetCameraData(view, proj);
                Render(ctx.Renderers, {0, 0, k_PointLightShadowMapSize, k_PointLightShadowMapSize}, "Point Light Shadow Pass " + std::to_string(pointLightsCount));
            }

            m_ShadowsData.PointLightShadows[pointLightsCount].Position = light->Position.ToVector4(0);

            ++pointLightsCount;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            depthTargetDescriptor.Layer = 0;

            Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
            Graphics::SetRenderTarget(depthTargetDescriptor, m_DirectionLightShadowMap);

            auto &bounds = ctx.ShadowCasterBounds;

            auto sizeWorldSpace   = bounds.GetSize();
            auto maxExtentWorldSpace = std::max({sizeWorldSpace.x, sizeWorldSpace.y, sizeWorldSpace.z});
            auto lightDir            = light->Rotation * Vector3 {0, 0, 1};
            auto viewPos             = bounds.GetCenter() - lightDir * maxExtentWorldSpace;
            auto viewMatrix          = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-viewPos);

            auto boundsViewSpace    = viewMatrix * bounds;
            auto extentsViewSpace   = boundsViewSpace.GetExtents();
            auto maxExtentViewSpace = std::max(extentsViewSpace.x, extentsViewSpace.y);
            auto projMatrix         = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0, ctx.ShadowDistance);

            Graphics::SetCameraData(viewMatrix, projMatrix);

            m_ShadowsData.DirectionalLightViewProjMatrix = biasMatrix * projMatrix * viewMatrix;

            Render(ctx.Renderers, {0, 0, k_DirLightShadowMapSize, k_DirLightShadowMapSize}, "Directional Light Shadow Pass");
        }
    }

    m_ShadowsConstantBuffer->SetData(&m_ShadowsData, 0, sizeof(ShadowsData));
}

void ShadowCasterPass::Render(const std::vector<std::shared_ptr<Renderer>> &_renderers, const Vector4& viewport, const std::string& passName)
{
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/shadowCaster", {}, {}, {}, {});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "ShadowCaster");
    static const RenderSettings renderSettings {DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), material};

    GraphicsBackend::Current()->BeginRenderPass(passName);
    Graphics::SetViewport(viewport);
    Graphics::DrawRenderers(_renderers, renderSettings);
    GraphicsBackend::Current()->EndRenderPass();
}
