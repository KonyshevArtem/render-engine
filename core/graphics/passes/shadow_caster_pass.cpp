#include "shadow_caster_pass.h"
#include "graphics/graphics.h"
#include "graphics/render_data.h"
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
#include "types/graphics_backend_buffer_descriptor.h"
#include "resources/resources.h"
#include "editor/texture_viewer/texture_viewer.h"

#include <cfloat>

namespace ShadowCasterPassLocal
{
    constexpr int k_PunctualLightShadowMapSize = 512;
    constexpr int k_PunctualLightShadowAtlasSlots = 32;
    constexpr int k_DirLightShadowMapSize = 2048;

    struct ShadowCasterPassData
    {
        Vector4 LightPosWS;

        Vector3 Padding0;
        float ShadowDepthBias;
    };
}

ShadowCasterPass::ShadowCasterPass() :
    RenderPass(),
	m_BiasMatrix(Matrix4x4::TRS(Vector3{ 0.5f, 0.5f, 0.5f }, Quaternion(), Vector3{ 0.5f, 0.5f, 0.5f })),
    m_Shader(Resources::LoadShader("core_resources/shaders/shadowCaster", {})),
	m_Material(std::make_shared<Material>(m_Shader, "ShadowCaster"))
{
    m_PunctualShadowAtlasSlots.reserve(ShadowCasterPassLocal::k_PunctualLightShadowAtlasSlots);
    for (int i = ShadowCasterPassLocal::k_PunctualLightShadowAtlasSlots - 1; i >= 0; --i)
        m_PunctualShadowAtlasSlots.push_back(i);
}

void ShadowCasterPass::Prepare(RenderData& renderData)
{
    const RenderSettings punctualLightRenderSettings{DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), m_Material};

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

    GraphicsBackendTextureDescriptor shadowMapDescriptor{};
    shadowMapDescriptor.Format = TextureInternalFormat::DEPTH_32;
    shadowMapDescriptor.Linear = true;
    shadowMapDescriptor.RenderTarget = true;

    if (!m_DirectionLightShadowMap)
    {
        shadowMapDescriptor.Width = ShadowCasterPassLocal::k_DirLightShadowMapSize;
        shadowMapDescriptor.Height = ShadowCasterPassLocal::k_DirLightShadowMapSize;
        shadowMapDescriptor.Depth = GlobalConstants::ShadowCascadeCount;

        m_DirectionLightShadowMap = Texture2DArray::Create(shadowMapDescriptor, "DirectionalShadowMap");
        m_DirectionLightShadowMap->SetWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
        m_DirectionLightShadowMap->SetFilteringMode(TextureFilteringMode::LINEAR);
        m_DirectionLightShadowMap->SetComparisonFunction(ComparisonFunction::LEQUAL);
    }

    if (!m_PunctualLightShadowAtlas)
    {
        shadowMapDescriptor.Width = ShadowCasterPassLocal::k_PunctualLightShadowMapSize;
        shadowMapDescriptor.Height = ShadowCasterPassLocal::k_PunctualLightShadowMapSize;
        shadowMapDescriptor.Depth = ShadowCasterPassLocal::k_PunctualLightShadowAtlasSlots;

        m_PunctualLightShadowAtlas = Texture2DArray::Create(shadowMapDescriptor, "PunctualLightShadowMapAtlas");
        m_PunctualLightShadowAtlas->SetBorderColor({ 1, 1, 1, 1 });
        m_PunctualLightShadowAtlas->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
        m_PunctualLightShadowAtlas->SetFilteringMode(TextureFilteringMode::LINEAR);
        m_PunctualLightShadowAtlas->SetComparisonFunction(ComparisonFunction::LEQUAL);
    }

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;

    if (!m_ShadowsConstantBuffer)
    {
        bufferDescriptor.Size = sizeof(ShadowsData);
        m_ShadowsConstantBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "ShadowsData");
    }

    if (!m_ShadowCasterPassBuffer)
    {
        bufferDescriptor.Size = sizeof(ShadowCasterPassLocal::ShadowCasterPassData) * 128;
        m_ShadowCasterPassBuffer = std::make_shared<RingBuffer>(bufferDescriptor, "ShadowCasterPassBuffer");
    }

    const float shadowsDistance = GraphicsSettings::GetShadowDistance();

    uint8_t spotLightIndex = 0;
    uint8_t pointLightsIndex = 0;
    for (Light* light : renderData.Lights)
    {
        if (light == nullptr)
            continue;

        std::shared_ptr<GameObject> lightGo = light->GetGameObject();
        if (light->Type == LightType::SPOT && spotLightIndex < GlobalConstants::MaxSpotLightSources)
        {
            if (!TryReserveShadowAtlasSlots(*light))
                continue;

            Profiler::Marker marker("Prepare Spot Light");

            const float farPlane = std::min(light->Range, shadowsDistance);
            const Matrix4x4 view = Matrix4x4::Rotation(lightGo->GetRotation().Inverse()) * Matrix4x4::Translation(-lightGo->GetPosition());
            const Matrix4x4 proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, farPlane);
            const Matrix4x4 viewProj = proj * view;

            m_SpotLightCameraData[spotLightIndex] = {view, proj, lightGo->GetPosition().ToVector4(1), farPlane, light->PunctualShadowAtlasSlots[0]};
            m_SpotLightRenderQueues[spotLightIndex].Prepare(viewProj, renderData.Renderers, punctualLightRenderSettings);

            m_ShadowsGPUData.SpotLightShadows[spotLightIndex].ViewProjMatrix = m_BiasMatrix * viewProj;
            m_ShadowsGPUData.SpotLightShadows[spotLightIndex].ShadowAtlasSlot = light->PunctualShadowAtlasSlots[0];

            ++spotLightIndex;
        }
        if (light->Type == LightType::POINT && pointLightsIndex < GlobalConstants::MaxPointLightSources)
        {
            if (!TryReserveShadowAtlasSlots(*light))
                continue;

            Profiler::Marker marker("Prepare Point Light");

            const float farPlane = std::min(light->Range, shadowsDistance);
            const Matrix4x4 proj = Matrix4x4::Perspective(90, 1, 0.01f, farPlane);
            for (int i = 0; i < 6; ++i)
            {
                const Matrix4x4 view = pointLightViewMatrices[i] * Matrix4x4::Translation(-lightGo->GetPosition());
                const Matrix4x4 viewProj = proj * view;

                m_PointLightCameraData[pointLightsIndex * 6 + i] = {view, proj, lightGo->GetPosition().ToVector4(1), farPlane, light->PunctualShadowAtlasSlots[i]};
                m_PointLightsRenderQueues[pointLightsIndex * 6 + i].Prepare(viewProj, renderData.Renderers, punctualLightRenderSettings);
                m_ShadowsGPUData.PointLightShadows[pointLightsIndex].ViewProjMatrices[i] = m_BiasMatrix * viewProj;
                m_ShadowsGPUData.PointLightShadows[pointLightsIndex].ShadowAtlasSlots[i] = light->PunctualShadowAtlasSlots[i];
            }

            m_ShadowsGPUData.PointLightShadows[pointLightsIndex].Position = lightGo->GetPosition().ToVector4(0);

            ++pointLightsIndex;
        }
        else if (light->Type == LightType::DIRECTIONAL)
        {
            if (renderData.RaytracedShadowsEnabled)
            {
                m_DirectionLightShadowMap = nullptr;
                continue;
            }

            Profiler::Marker marker("Prepare Directional Light");

            std::shared_ptr<Worker::Task> cascadesPrepareTask = std::make_shared<Worker::Task>();
            for (uint32_t i = GlobalConstants::ShadowCascadeCount - 1; i > 0; --i)
            {
                std::shared_ptr<Worker::Task> task = Worker::CreateTask([this, i, &renderData, &lightGo] { PrepareCascade(i, renderData, lightGo); }, Worker::Priority::TASK);
                cascadesPrepareTask->AddDependency(task);

                if (!Graphics::IsPrepareSynchronous())
                    task->Schedule();
            }

            if (!Graphics::IsPrepareSynchronous())
                cascadesPrepareTask->Schedule();

            PrepareCascade(0, renderData, lightGo);

            if (Graphics::IsPrepareSynchronous())
                cascadesPrepareTask->Execute();
            else
                cascadesPrepareTask->Wait();
        }
    }
}

void ShadowCasterPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("ShadowCasterPass::Execute");

    if (!m_Shader || !m_Shader->IsValid())
		return;

    m_ShadowsConstantBuffer->SetData(&m_ShadowsGPUData, 0, sizeof(ShadowsData));
    GraphicsBackend::Current()->BindConstantBuffer(m_ShadowsConstantBuffer->GetBackendBuffer(), GlobalConstants::ConstantBufferIndex::SHADOW_DATA, 0, sizeof(ShadowsData));

    for (int i = 0; i < GlobalConstants::MaxSpotLightSources; ++i)
    {
        if (m_SpotLightRenderQueues[i].IsEmpty())
            break;

        Render(m_SpotLightRenderQueues[i], m_PunctualLightShadowAtlas, m_SpotLightCameraData[i], "Spot Light Shadow Pass " + std::to_string(i));
    }

    for (int i = 0; i < GlobalConstants::MaxPointLightSources; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            const int viewIndex = i * 6 + j;
            if (!m_PointLightsRenderQueues[viewIndex].IsEmpty())
                Render(m_PointLightsRenderQueues[viewIndex], m_PunctualLightShadowAtlas, m_PointLightCameraData[viewIndex], "Point Light Shadow Pass " + std::to_string(i));
        }
    }

    for (int i = 0; i < GlobalConstants::ShadowCascadeCount; ++i)
    {
        if (!m_DirectionalLightRenderQueues[i].IsEmpty())
            Render(m_DirectionalLightRenderQueues[i], m_DirectionLightShadowMap, m_DirectionLightCameraData[i], "Directional Light Shadow Pass " + std::to_string(i));
    }

    TextureViewer::RegisterTexture(m_DirectionLightShadowMap, "Shadows/DirectionalShadowMap");
    TextureViewer::RegisterTexture(m_PunctualLightShadowAtlas, "Shadows/PunctualLightShadowAtlas");
}

void ShadowCasterPass::BindShadowMaps() const
{
	if (m_DirectionLightShadowMap)
		GraphicsBackend::Current()->BindTextureSampler(m_DirectionLightShadowMap->GetBackendTexture(), m_DirectionLightShadowMap->GetBackendSampler(), GlobalConstants::TextureIndex::DIRECTIONAL_SHADOW_MAP);
    GraphicsBackend::Current()->BindTextureSampler(m_PunctualLightShadowAtlas->GetBackendTexture(), m_PunctualLightShadowAtlas->GetBackendSampler(), GlobalConstants::TextureIndex::PUNCTUAL_LIGHT_SHADOW_ATLAS);
}

void ShadowCasterPass::Render(RenderQueue& renderQueue, const std::shared_ptr<Texture>& target, const ShadowsCameraData& cameraData, const std::string& passName) const
{
    static constexpr GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::DONT_CARE, .StoreAction = StoreAction::DONT_CARE };

    Profiler::Marker marker("ShadowCasterPass::Render");

    const GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .Texture = target->GetBackendTexture(), .LoadAction = LoadAction::CLEAR, .Layer = static_cast<int>(cameraData.ShadowMapLayer) };

    GraphicsBackend::Current()->AttachRenderTarget(colorTargetDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(depthTargetDescriptor);

    ShadowCasterPassLocal::ShadowCasterPassData data;
    data.LightPosWS = cameraData.LightPosOrDir;
    data.ShadowDepthBias = GraphicsSettings::GetShadowDepthBias();
    const uint64_t offset = m_ShadowCasterPassBuffer->SetData(&data, 0, sizeof(data));

    Graphics::SetCameraData(cameraData.ViewMatrix, cameraData.ProjectionMatrix, 0.01f, cameraData.FarPlane);

    GraphicsBackend::Current()->BeginRenderPass(passName);
    {
        Profiler::GPUMarker gpuMarker("ShadowCasterPass::Render");

        GraphicsBackend::Current()->BindConstantBuffer(m_ShadowCasterPassBuffer->GetBackendBuffer(), 0, offset, sizeof(data));

        GraphicsBackend::Current()->SetViewport(0, 0, target->GetWidth(), target->GetHeight(), 0, 1);
        GraphicsBackend::Current()->SetScissorRect(0, 0, target->GetWidth(), target->GetHeight());

        renderQueue.Draw();
    }
    GraphicsBackend::Current()->EndRenderPass();
}

void ShadowCasterPass::PrepareCascade(uint32_t cascade, RenderData& renderData, const std::shared_ptr<GameObject>& lightGameObject)
{
    Profiler::Marker _("ShadowCasterPass::PrepareCascade");

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

    const Matrix4x4 rotationViewMatrix = Matrix4x4::Rotation(lightGameObject->GetRotation().Inverse());

    const float shadowsDistance = GraphicsSettings::GetShadowDistance();
    const float cascadeNear = cascade > 0 ? shadowsDistance * GraphicsSettings::GetShadowCascadeBounds(cascade - 1) : renderData.NearPlane;
    const float cascadeFar = shadowsDistance * GraphicsSettings::GetShadowCascadeBounds(cascade);

    const Matrix4x4 invCameraVP = (Matrix4x4::Perspective(renderData.FoV, renderData.Viewport.x / renderData.Viewport.y, cascadeNear, cascadeFar) * renderData.ViewMatrix).Invert();

    Vector3 viewMin(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 viewMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const Vector3& corner : corners)
    {
        Vector4 worldPos = invCameraVP * corner.ToVector4(1);
        worldPos /= worldPos.w;

        Vector4 viewPos = rotationViewMatrix * worldPos;
        viewMin = Vector3::Min(viewMin, viewPos);
        viewMax = Vector3::Max(viewMax, viewPos);
    }

    const Vector3 viewOffset = (viewMin + viewMax) * 0.5f;
    const Matrix4x4 cullingViewMatrix = Matrix4x4::Translation({ -viewOffset.x, -viewOffset.y, -viewMin.z }) * rotationViewMatrix;

    const Vector3 viewExtents = (viewMax - viewMin) * 0.5f;
    const float maxExtentViewSpace = std::max(viewExtents.x, viewExtents.y);
    const Matrix4x4 cullingProjMatrix = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0.01f, viewMax.z - viewMin.z);

    const RenderSettings dirLightShadowRenderSettings{ DrawCallSortMode::NO_SORTING, DrawCallFilter::ShadowCasters(), m_Material, Frustum::SidePlanesBits };
    m_DirectionalLightRenderQueues[cascade].Prepare(cullingProjMatrix * cullingViewMatrix, renderData.Renderers, dirLightShadowRenderSettings);

    const std::vector<DrawCallInfo>& dirLightShadowDrawCalls = m_DirectionalLightRenderQueues[cascade].GetDrawCalls();
    for (const DrawCallInfo& drawCallInfo : dirLightShadowDrawCalls)
    {
        Bounds projectedBounds = rotationViewMatrix * drawCallInfo.AABB;
        viewMin.z = std::min(viewMin.z, projectedBounds.Min.z);
        viewMax.z = std::max(viewMax.z, projectedBounds.Max.z);
    }

    const float renderFarPlane = viewMax.z - viewMin.z;
    const Matrix4x4 renderViewMatrix = Matrix4x4::Translation({ -viewOffset.x, -viewOffset.y, -viewMin.z }) * rotationViewMatrix;
    const Matrix4x4 renderProjMatrix = Matrix4x4::Orthographic(-maxExtentViewSpace, maxExtentViewSpace, -maxExtentViewSpace, maxExtentViewSpace, 0.01f, renderFarPlane);

    const Vector3 lightDirection = lightGameObject->GetRotation() * Vector3(0, 0, 1);
    m_DirectionLightCameraData[cascade] = { renderViewMatrix, renderProjMatrix, lightDirection.ToVector4(0), renderFarPlane, cascade };
    m_ShadowsGPUData.DirectionalLightViewProjMatrix[cascade] = m_BiasMatrix * renderProjMatrix * renderViewMatrix;
}

bool ShadowCasterPass::TryReserveShadowAtlasSlots(Light& light)
{
    if (!light.PunctualShadowAtlasSlots.empty())
	    return true;

    const uint32_t slotsCount = light.Type == LightType::POINT ? 6 : 1;
    if (m_PunctualShadowAtlasSlots.size() < slotsCount)
	    return false;

    light.PunctualShadowAtlasSlots.reserve(slotsCount);
    for (uint32_t i = 0; i < slotsCount; ++i)
    {
	    light.PunctualShadowAtlasSlots.push_back(m_PunctualShadowAtlasSlots.back());
	    m_PunctualShadowAtlasSlots.pop_back();
    }

    return true;
}
