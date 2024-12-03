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

void ShadowCasterPass::Prepare(const std::vector<std::shared_ptr<Renderer>>& renderers, const std::vector<std::shared_ptr<Light>>& lights, float shadowsDistance)
{
    static const Matrix4x4 biasMatrix = Matrix4x4::TRS(Vector3{0.5f, 0.5f, 0.5f}, Quaternion(), Vector3{0.5f, 0.5f, 0.5f});
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/shadowCaster", {}, {}, {}, {});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "ShadowCaster");
    static const RenderSettings renderSettings {DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), material};

    static Matrix4x4 pointLightViewMatrices[6]
    {
        Matrix4x4::TBN({0, 0, 1}, {0, 1, 0}, {-1, 0, 0}).Invert(), // right
        Matrix4x4::TBN({0, 0, -1}, {0, 1, 0}, {1, 0, 0}).Invert(), // left
        Matrix4x4::TBN({1, 0, 0}, {0, 0, 1}, {0, -1, 0}).Invert(), // up
        Matrix4x4::TBN({1, 0, 0}, {0, 0, -1}, {0, 1, 0}).Invert(), // down
        Matrix4x4::TBN({1, 0, 0}, {0, 1, 0}, {0, 0, 1}).Invert(), // forward
        Matrix4x4::TBN({-1, 0, 0}, {0, 1, 0}, {0, 0, -1}).Invert(), // back
    };

    m_DirectionalLightRenderQueue.Clear();
    for (RenderQueue& queue : m_PointLightsRenderQueues)
        queue.Clear();
    for (RenderQueue& queue : m_SpotLightRenderQueues)
        queue.Clear();

    uint8_t spotLightIndex = 0;
    uint8_t pointLightsIndex = 0;
    for (const std::shared_ptr<Light>& light : lights)
    {
        if (light == nullptr)
            continue;

        if (light->Type == LightType::SPOT)
        {
            const Matrix4x4 view = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
            const Matrix4x4 proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, shadowsDistance);

            m_SpotLightMatrices[spotLightIndex] = {view, proj};
            m_SpotLightRenderQueues[spotLightIndex].Prepare(light->Position, renderers, renderSettings);
            m_ShadowsGPUData.SpotLightsViewProjMatrices[spotLightIndex] = biasMatrix * proj * view;

            ++spotLightIndex;
        }
        if (light->Type == LightType::POINT)
        {
            const Matrix4x4 proj = Matrix4x4::Perspective(90, 1, 0.01f, shadowsDistance);
            for (int i = 0; i < 6; ++i)
            {
                const Matrix4x4 view = pointLightViewMatrices[i] * Matrix4x4::Translation(-light->Position);

                m_PointLightMatrices[pointLightsIndex * 6 + i] = {view, proj};
                m_PointLightsRenderQueues[pointLightsIndex].Prepare(light->Position, renderers, renderSettings);
                m_ShadowsGPUData.PointLightShadows[pointLightsIndex].ViewProjMatrices[i] = biasMatrix * proj * view;
            }

            m_ShadowsGPUData.PointLightShadows[pointLightsIndex].Position = light->Position.ToVector4(0);

            ++pointLightsIndex;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            m_DirectionalLightRenderQueue.Prepare({}, renderers, renderSettings);

            Bounds shadowCastersBounds;
            const std::vector<DrawCallInfo>& dirLightShadowDrawCalls = m_DirectionalLightRenderQueue.GetDrawCalls();
            for (int i = 0; i < dirLightShadowDrawCalls.size(); ++i)
            {
                if (i == 0)
                    shadowCastersBounds = dirLightShadowDrawCalls[i].AABB;
                else
                    shadowCastersBounds = dirLightShadowDrawCalls[i].AABB.Combine(shadowCastersBounds);
            }

            const Vector3 sizeWorldSpace = shadowCastersBounds.GetSize();
            const float maxExtentWorldSpace = std::max({sizeWorldSpace.x, sizeWorldSpace.y, sizeWorldSpace.z});
            const Vector3 lightDir = light->Rotation * Vector3{0, 0, 1};
            const Vector3 viewPos = shadowCastersBounds.GetCenter() - lightDir * maxExtentWorldSpace;
            const Matrix4x4 viewMatrix = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-viewPos);

            const Bounds boundsViewSpace = viewMatrix * shadowCastersBounds;
            const Vector3 extentsViewSpace = boundsViewSpace.GetExtents();
            const float maxExtentViewSpace = std::max(extentsViewSpace.x, extentsViewSpace.y);
            const Matrix4x4 projMatrix = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0, shadowsDistance);

            m_DirectionLightMatrices = {viewMatrix, projMatrix};
            m_ShadowsGPUData.DirectionalLightViewProjMatrix = biasMatrix * projMatrix * viewMatrix;
        }
    }

    m_ShadowsConstantBuffer->SetData(&m_ShadowsGPUData, 0, sizeof(ShadowsData));
}

void ShadowCasterPass::Execute(const Context& ctx)
{
    for (int i = 0; i < GlobalConstants::MaxSpotLightSources; ++i)
    {
        if (m_SpotLightRenderQueues[i].IsEmpty())
            break;

        Render(m_SpotLightRenderQueues[i], m_SpotLightShadowMapArray, i, m_SpotLightMatrices[i], "Spot Light Shadow Pass " + std::to_string(i));
    }

    for (int i = 0; i < GlobalConstants::MaxPointLightSources; ++i)
    {
        if (m_PointLightsRenderQueues[i].IsEmpty())
            break;

        for (int j = 0; j < 6; ++j)
        {
            const int viewIndex = i * 6 + j;
            Render(m_PointLightsRenderQueues[i], m_PointLightShadowMap, viewIndex, m_PointLightMatrices[viewIndex], "Point Light Shadow Pass " + std::to_string(i));
        }
    }

    if (!m_DirectionalLightRenderQueue.IsEmpty())
        Render(m_DirectionalLightRenderQueue, m_DirectionLightShadowMap, 0, m_DirectionLightMatrices, "Directional Light Shadow Pass");
}

void ShadowCasterPass::Render(const RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, int targetLayer, const ShadowsMatrices& matrices, const std::string& passName)
{
    static constexpr GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::DONT_CARE, .StoreAction = StoreAction::DONT_CARE };

    const Vector4& viewport{0, 0, static_cast<float>(target->GetWidth()), static_cast<float>(target->GetHeight())};
    const GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR, .Layer = targetLayer };

    Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
    Graphics::SetRenderTarget(depthTargetDescriptor, target);
    Graphics::SetCameraData(matrices.ViewMatrix, matrices.ProjectionMatrix);

    GraphicsBackend::Current()->BeginRenderPass(passName);
    Graphics::SetViewport(viewport);
    Graphics::DrawRenderQueue(renderQueue);
    GraphicsBackend::Current()->EndRenderPass();
}
