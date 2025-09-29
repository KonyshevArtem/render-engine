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
#include "editor/profiler/profiler.h"
#include "enums/framebuffer_attachment.h"
#include "enums/load_action.h"
#include "material/material.h"
#include "shader/shader.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics_settings.h"
#include "editor/gizmos/gizmos.h"
#include "input/input.h"

#include <cfloat>

constexpr int k_SpotLightShadowMapSize = 1024;
constexpr int k_DirLightShadowMapSize = 2048;
constexpr int k_PointLightShadowMapSize = 512;

ShadowCasterPass::ShadowCasterPass(std::shared_ptr<GraphicsBuffer> shadowsConstantBuffer, int priority) :
    RenderPass(priority),
    m_ShadowsConstantBuffer(std::move(shadowsConstantBuffer)),
    m_SpotLightShadowMapArray(Texture2DArray::Create(k_SpotLightShadowMapSize, k_SpotLightShadowMapSize, GlobalConstants::MaxSpotLightSources, TextureInternalFormat::DEPTH_32, true, "SpotLightShadowMap")),
    m_DirectionLightShadowMap(Texture2DArray::Create(k_DirLightShadowMapSize, k_DirLightShadowMapSize, GlobalConstants::ShadowCascadeCount, TextureInternalFormat::DEPTH_32, true, "DirectionalShadowMap")),
    m_PointLightShadowMap(Texture2DArray::Create(k_PointLightShadowMapSize, k_PointLightShadowMapSize, GlobalConstants::MaxPointLightSources * 6, TextureInternalFormat::DEPTH_32, true, "PointLightShadowMap"))
{
    Graphics::SetGlobalTexture("_DirLightShadowMap", m_DirectionLightShadowMap);
    Graphics::SetGlobalTexture("_SpotLightShadowMapArray", m_SpotLightShadowMapArray);
    Graphics::SetGlobalTexture("_PointLightShadowMapArray", m_PointLightShadowMap);

    m_DirectionLightShadowMap->SetBorderColor({1, 1, 1, 1});
    m_DirectionLightShadowMap->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    m_DirectionLightShadowMap->SetFilteringMode(TextureFilteringMode::LINEAR);
    m_DirectionLightShadowMap->SetComparisonFunction(ComparisonFunction::LEQUAL);

    m_SpotLightShadowMapArray->SetBorderColor({1, 1, 1, 1});
    m_SpotLightShadowMapArray->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    m_SpotLightShadowMapArray->SetFilteringMode(TextureFilteringMode::LINEAR);
    m_SpotLightShadowMapArray->SetComparisonFunction(ComparisonFunction::LEQUAL);

    m_PointLightShadowMap->SetBorderColor({1, 1, 1, 1});
    m_PointLightShadowMap->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    m_PointLightShadowMap->SetFilteringMode(TextureFilteringMode::LINEAR);
    m_PointLightShadowMap->SetComparisonFunction(ComparisonFunction::LEQUAL);
}

void ShadowCasterPass::Prepare(const Context& ctx)
{
    static const Matrix4x4 biasMatrix = Matrix4x4::TRS(Vector3{0.5f, 0.5f, 0.5f}, Quaternion(), Vector3{0.5f, 0.5f, 0.5f});
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/shadowCaster", {}, {}, {}, {});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "ShadowCaster");
    static const RenderSettings dirLightShadowRenderSettings {DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), material, Frustum::SidePlanesBits};
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

    Profiler::Marker marker("ShadowCasterPass::Prepare");

    for (RenderQueue& queue : m_DirectionalLightRenderQueues)
        queue.Clear();
    for (RenderQueue& queue : m_PointLightsRenderQueues)
        queue.Clear();
    for (RenderQueue& queue : m_SpotLightRenderQueues)
        queue.Clear();

    const float shadowsDistance = GraphicsSettings::GetShadowDistance();

    uint8_t spotLightIndex = 0;
    uint8_t pointLightsIndex = 0;
    for (Light* light : ctx.Lights)
    {
        if (light == nullptr)
            continue;

        std::shared_ptr<GameObject> lightGo = light->GetGameObject();
        if (light->Type == LightType::SPOT)
        {
            Profiler::Marker marker("Prepare Spot Light");

            const float farPlane = std::min(light->Range, shadowsDistance);
            const Matrix4x4 view = Matrix4x4::Rotation(lightGo->GetRotation().Inverse()) * Matrix4x4::Translation(-lightGo->GetPosition());
            const Matrix4x4 proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, farPlane);
            const Matrix4x4 viewProj = proj * view;

            m_SpotLightCameraData[spotLightIndex] = {view, proj, farPlane};
            m_SpotLightRenderQueues[spotLightIndex].Prepare(viewProj, ctx.Renderers, renderSettings);
            m_ShadowsGPUData.SpotLightsViewProjMatrices[spotLightIndex] = biasMatrix * viewProj;

            ++spotLightIndex;
        }
        if (light->Type == LightType::POINT)
        {
            Profiler::Marker marker("Prepare Point Light");

            const float farPlane = std::min(light->Range, shadowsDistance);
            const Matrix4x4 proj = Matrix4x4::Perspective(90, 1, 0.01f, farPlane);
            for (int i = 0; i < 6; ++i)
            {
                const Matrix4x4 view = pointLightViewMatrices[i] * Matrix4x4::Translation(-lightGo->GetPosition());
                const Matrix4x4 viewProj = proj * view;

                m_PointLightCameraData[pointLightsIndex * 6 + i] = {view, proj, farPlane};
                m_PointLightsRenderQueues[pointLightsIndex * 6 + i].Prepare(viewProj, ctx.Renderers, renderSettings);
                m_ShadowsGPUData.PointLightShadows[pointLightsIndex].ViewProjMatrices[i] = biasMatrix * viewProj;
            }

            m_ShadowsGPUData.PointLightShadows[pointLightsIndex].Position = lightGo->GetPosition().ToVector4(0);

            ++pointLightsIndex;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            Profiler::Marker marker("Prepare Directional Light");

            const Vector3 corners[8] =
            {
                {-1, -1, -1},
                {-1, -1, 1},
                {-1, 1, -1},
                {-1, 1, 1},
                {1, -1, -1},
                {1, -1, 1},
                {1, 1, -1},
                {1, 1, 1},
            };

            const Matrix4x4 rotationViewMatrix = Matrix4x4::Rotation(lightGo->GetRotation().Inverse());

            for (int i = 0; i < GlobalConstants::ShadowCascadeCount; ++i)
            {
                const float cascadeNear = i > 0 ? shadowsDistance * GraphicsSettings::GetShadowCascadeBounds(i - 1) : ctx.NearPlane;
                const float cascadeFar = shadowsDistance * GraphicsSettings::GetShadowCascadeBounds(i);

                const Matrix4x4 invCameraVP = (Matrix4x4::Perspective(ctx.FoV, ctx.Viewport.x / ctx.Viewport.y, cascadeNear, cascadeFar) * ctx.ViewMatrix).Invert();

                Vector3 viewMin(FLT_MAX, FLT_MAX, FLT_MAX);
                Vector3 viewMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

                for (int j = 0; j < 8; ++j)
                {
                    Vector4 worldPos = invCameraVP * corners[j].ToVector4(1);
                    worldPos /= worldPos.w;

                    Vector4 viewPos = rotationViewMatrix * worldPos;
                    viewMin = Vector3::Min(viewMin, viewPos);
                    viewMax = Vector3::Max(viewMax, viewPos);
                }

                const Vector3 viewOffset = (viewMin + viewMax) * 0.5f;
                const Matrix4x4 cullingViewMatrix = Matrix4x4::Translation({-viewOffset.x, -viewOffset.y, -viewMin.z}) * rotationViewMatrix;

                const Vector3 viewExtents = (viewMax - viewMin) * 0.5f;
                const float maxExtentViewSpace = std::max(viewExtents.x, viewExtents.y);
                const Matrix4x4 cullingProjMatrix = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0.01, viewMax.z - viewMin.z);

                m_DirectionalLightRenderQueues[i].Prepare(cullingProjMatrix * cullingViewMatrix, ctx.Renderers, dirLightShadowRenderSettings);

                const std::vector<DrawCallInfo> &dirLightShadowDrawCalls = m_DirectionalLightRenderQueues[i].GetDrawCalls();
                for (int j = 0; j < dirLightShadowDrawCalls.size(); ++j)
                {
                    Bounds projectedBounds = rotationViewMatrix * dirLightShadowDrawCalls[j].AABB;
                    viewMin.z = std::min(viewMin.z, projectedBounds.Min.z);
                    viewMax.z = std::max(viewMax.z, projectedBounds.Max.z);
                }

                const float renderFarPlane = viewMax.z - viewMin.z;
                const Matrix4x4 renderViewMatrix = Matrix4x4::Translation({-viewOffset.x, -viewOffset.y, -viewMin.z}) * rotationViewMatrix;
                const Matrix4x4 renderProjMatrix = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0.01, renderFarPlane);

                m_DirectionLightCameraData[i] = {renderViewMatrix, renderProjMatrix, renderFarPlane};
                m_ShadowsGPUData.DirectionalLightViewProjMatrix[i] = biasMatrix * renderProjMatrix * renderViewMatrix;
            }
        }
    }

    m_ShadowsConstantBuffer->SetData(&m_ShadowsGPUData, 0, sizeof(ShadowsData));
}

void ShadowCasterPass::Execute(const Context& ctx)
{
    Profiler::Marker marker("ShadowCasterPass::Execute");

    for (int i = 0; i < GlobalConstants::MaxSpotLightSources; ++i)
    {
        if (m_SpotLightRenderQueues[i].IsEmpty())
            break;

        Render(m_SpotLightRenderQueues[i], m_SpotLightShadowMapArray, i, m_SpotLightCameraData[i], "Spot Light Shadow Pass " + std::to_string(i));
    }

    for (int i = 0; i < GlobalConstants::MaxPointLightSources; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            const int viewIndex = i * 6 + j;
            if (!m_PointLightsRenderQueues[viewIndex].IsEmpty())
                Render(m_PointLightsRenderQueues[viewIndex], m_PointLightShadowMap, viewIndex, m_PointLightCameraData[viewIndex], "Point Light Shadow Pass " + std::to_string(i));
        }
    }

    for (int i = 0; i < GlobalConstants::ShadowCascadeCount; ++i)
    {
        if (!m_DirectionalLightRenderQueues[i].IsEmpty())
            Render(m_DirectionalLightRenderQueues[i], m_DirectionLightShadowMap, i, m_DirectionLightCameraData[i], "Directional Light Shadow Pass " + std::to_string(i));
    }
}

void ShadowCasterPass::Render(const RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, int targetLayer, const ShadowsCameraData& cameraData, const std::string& passName)
{
    static constexpr GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::DONT_CARE, .StoreAction = StoreAction::DONT_CARE };

    Profiler::Marker marker("ShadowCasterPass::Render");
    Profiler::GPUMarker gpuMarker("ShadowCasterPass::Render");

    const Vector4& viewport{0, 0, static_cast<float>(target->GetWidth()), static_cast<float>(target->GetHeight())};
    const GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR, .Layer = targetLayer };

    Graphics::SetRenderTarget(colorTargetDescriptor, nullptr);
    Graphics::SetRenderTarget(depthTargetDescriptor, target);
    Graphics::SetCameraData(cameraData.ViewMatrix, cameraData.ProjectionMatrix, 0.01, cameraData.FarPlane);

    GraphicsBackend::Current()->BeginRenderPass(passName);
    Graphics::SetViewport(viewport);
    Graphics::DrawRenderQueue(renderQueue);
    GraphicsBackend::Current()->EndRenderPass();
}
