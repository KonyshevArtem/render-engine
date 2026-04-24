#include "graphics.h"
#include "camera/camera.h"
#include "render_data.h"
#include "draw_call_info.h"
#include "editor/gizmos/gizmos.h"
#include "editor/gizmos/gizmos_pass.h"
#include "light/light.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "enums/indices_data_type.h"
#include "shader/shader.h"
#include "data_structs/camera_data.h"
#include "data_structs/lighting_data.h"
#include "types/graphics_backend_buffer_info.h"
#include "texture_2d/texture_2d.h"
#include "mesh/mesh.h"
#include "passes/final_blit_pass.h"
#include "editor/selection_outline/selection_outline_pass.h"
#include "graphics_settings.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "material/material.h"
#include "passes/forward_render_pass.h"
#include "editor/copy_depth/copy_depth_pass.h"
#include "render_queue/render_queue.h"
#include "editor/profiler/profiler.h"
#include "editor/debug_pass/shadow_map_debug_pass.h"
#include "cubemap/cubemap.h"
#include "gameObject/gameObject.h"
#include "ui/ui_render_pass.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics_buffer/ring_buffer.h"
#include "global_constants.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "graphics_buffer/graphics_buffer_view.h"
#include "passes/post_process_pass.h"
#include "developer_console/developer_console.h"
#include "raytracing/raytracing_scene.h"
#include "raytracing/raytracing_pass.h"
#include "passes/gbuffer_pass.h"
#include "passes/deferred_light_pass.h"
#include "passes/skybox_pass.h"
#include "arguments.h"

#include <cassert>

namespace Graphics
{
    std::shared_ptr<GraphicsBuffer> s_LightingDataBuffer;
    std::shared_ptr<RingBuffer> s_CameraDataBuffer;

    std::shared_ptr<ForwardRenderPass> s_ForwardRenderPass;
    std::shared_ptr<SkyboxPass> s_SkyboxPass;
	std::shared_ptr<GBufferPass> s_GBufferPass;
    std::shared_ptr<DeferredLightPass> s_DeferredLightPass;
    std::shared_ptr<ShadowCasterPass> s_ShadowCasterPass;
    std::shared_ptr<FinalBlitPass> s_FinalBlitPass;
    std::shared_ptr<UIRenderPass> s_UIRenderPass;
    std::shared_ptr<PostProcessPass> s_PostProcessPass;
    std::shared_ptr<RaytracingPass> s_RaytracingPass;

#if RENDER_ENGINE_EDITOR
    std::shared_ptr<CopyDepthPass> s_CopyDepthPass;
    std::shared_ptr<GizmosPass> s_3DGizmosPass;
    std::shared_ptr<GizmosPass> s_2DGizmosPass;
    std::shared_ptr<SelectionOutlinePass> s_SelectionOutlinePass;
    std::shared_ptr<ShadowMapDebugPass> s_ShadowMapDebugPass;
#endif

    int s_ScreenWidth  = 0;
    int s_ScreenHeight = 0;
    RenderData s_RenderData;
    std::vector<std::shared_ptr<RenderPass>> s_RenderPasses;
    std::shared_ptr<Worker::Task> s_PrepareTask;
    bool s_SynchronousGraphicsPrepare = false;

    std::shared_ptr<RaytracingScene> s_RaytracingScene;

    void InitConstantBuffers()
    {
        GraphicsBackendBufferDescriptor descriptor{};
        descriptor.AllowCPUWrites = true;

        descriptor.Size = sizeof(CameraData) * 32;
        s_CameraDataBuffer = std::make_shared<RingBuffer>(descriptor, "CameraData");

        descriptor.Size = sizeof(LightingData);
        s_LightingDataBuffer = std::make_shared<GraphicsBuffer>(descriptor, "LightingData");
    }

    void InitPasses()
    {
        s_ShadowCasterPass = std::make_shared<ShadowCasterPass>();
		s_GBufferPass = std::make_shared<GBufferPass>();
		s_DeferredLightPass = std::make_shared<DeferredLightPass>();
        s_ForwardRenderPass = std::make_shared<ForwardRenderPass>();
		s_SkyboxPass = std::make_shared<SkyboxPass>();
        s_PostProcessPass = std::make_shared<PostProcessPass>();
        s_UIRenderPass = std::make_shared<UIRenderPass>();
        s_FinalBlitPass = std::make_shared<FinalBlitPass>();
#if RENDER_ENGINE_EDITOR
        s_ShadowMapDebugPass = std::make_shared<ShadowMapDebugPass>();
        s_3DGizmosPass = std::make_shared<GizmosPass>(GizmosPass::Mode::GIZMOS_3D);
        s_2DGizmosPass = std::make_shared<GizmosPass>(GizmosPass::Mode::GIZMOS_2D);
        s_SelectionOutlinePass = std::make_shared<SelectionOutlinePass>();
#endif

        if (GraphicsBackend::Current()->SupportsRaytracing())
        {
            s_RaytracingScene = std::make_shared<RaytracingScene>();
            s_RaytracingPass = std::make_shared<RaytracingPass>(s_RaytracingScene);
        }
    }

    void Init()
    {
#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitConstantBuffers();
        InitPasses();

        s_SynchronousGraphicsPrepare = Arguments::Contains("-sync_graphics_prepare") || GraphicsBackend::Current()->GetName() == GraphicsBackendName::OPENGL || GraphicsBackend::Current()->GetName() == GraphicsBackendName::GLES;
        DeveloperConsole::AddBoolCommand(L"Graphics.Prepare.Synchronous", &s_SynchronousGraphicsPrepare);
    }

    void Shutdown()
    {
        s_LightingDataBuffer = nullptr;
        s_CameraDataBuffer = nullptr;

        s_ForwardRenderPass = nullptr;
		s_SkyboxPass = nullptr;
		s_GBufferPass = nullptr;
		s_DeferredLightPass = nullptr;
        s_ShadowCasterPass = nullptr;
        s_FinalBlitPass = nullptr;
        s_UIRenderPass = nullptr;
        s_PostProcessPass = nullptr;

#if RENDER_ENGINE_EDITOR
        s_CopyDepthPass = nullptr;
        s_3DGizmosPass = nullptr;
        s_2DGizmosPass = nullptr;
        s_SelectionOutlinePass = nullptr;
        s_ShadowMapDebugPass = nullptr;
#endif
    }

    void SetLightingData(const std::vector<Light*>& lights, const std::shared_ptr<Texture>& skybox)
    {
        const std::shared_ptr<Texture> reflectionCube = skybox ? skybox : Cubemap::White();

        LightingData lightingData{};
        lightingData.AmbientLight = GraphicsSettings::GetAmbientLightColor() * GraphicsSettings::GetAmbientLightIntensity();
        lightingData.PointLightsCount = 0;
        lightingData.SpotLightsCount = 0;
        lightingData.HasDirectionalLight = -1;
        lightingData.ReflectionCubeMips = reflectionCube->GetMipLevels();

        for (Light* light : lights)
        {
            if (light == nullptr)
                continue;

            std::shared_ptr<GameObject> lightGo = light->GetGameObject();
            if (light->Type == LightType::DIRECTIONAL && lightingData.HasDirectionalLight <= 0)
            {
                lightingData.HasDirectionalLight = 1;
                lightingData.DirLightDirection = lightGo->GetRotation() * Vector3(0, 0, 1);
                lightingData.DirLightIntensity = GraphicsSettings::GetSunLightColor() * GraphicsSettings::GetSunLightIntensity();
            }
            else if (light->Type == LightType::POINT && lightingData.PointLightsCount < GlobalConstants::MaxPointLightSources)
            {
                lightingData.PointLightsData[lightingData.PointLightsCount].Position = lightGo->GetPosition().ToVector4(1);
                lightingData.PointLightsData[lightingData.PointLightsCount].Intensity = light->Intensity;
                lightingData.PointLightsData[lightingData.PointLightsCount].Range = light->Range;
                ++lightingData.PointLightsCount;
            }
            else if (light->Type == LightType::SPOT && lightingData.SpotLightsCount < GlobalConstants::MaxSpotLightSources)
            {
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Position = lightGo->GetPosition().ToVector4(1);
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Direction = lightGo->GetRotation() * Vector3(0, 0, 1);
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Intensity = light->Intensity;
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Range = light->Range;
                lightingData.SpotLightsData[lightingData.SpotLightsCount].CutOffCosine = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
                ++lightingData.SpotLightsCount;
            }
        }

        GraphicsBackend::Current()->BindTextureSampler(reflectionCube->GetBackendTexture(), reflectionCube->GetBackendSampler(), GlobalConstants::ReflectionCubeIndex);

        s_LightingDataBuffer->SetData(&lightingData, 0, sizeof(lightingData));
        GraphicsBackend::Current()->BindConstantBuffer(s_LightingDataBuffer->GetBackendBuffer(), GlobalConstants::LightingDataIndex, 0, sizeof(lightingData));
    }

    void Prepare(int width, int height)
    {
        if (width == 0 || height == 0)
            return;

        Profiler::Marker marker("Graphics::Prepare");

        s_ScreenWidth = width;
        s_ScreenHeight = height;

        s_RenderPasses.clear();
        s_RenderData = RenderData::GetRenderData(width, height);

        s_PrepareTask = std::make_shared<Worker::Task>();

        auto SchedulePrepareTask = [](const std::function<void()>& taskFunc, const std::span<const std::shared_ptr<Worker::Task>>& dependencies)
            {
                std::shared_ptr<Worker::Task> prepareTask = Worker::CreateTask(taskFunc, Worker::Priority::TASK);
                for (const std::shared_ptr<Worker::Task>& dep : dependencies)
	                prepareTask->AddDependency(dep);

                s_PrepareTask->AddDependency(prepareTask);

				if (!s_SynchronousGraphicsPrepare)
					prepareTask->Schedule();
                return prepareTask;
            };

        auto SchedulePassPrepare = [&](const std::shared_ptr<RenderPass>& renderPass, const std::span<const std::shared_ptr<Worker::Task>>& dependencies)
            {
                s_RenderPasses.push_back(renderPass);
                return SchedulePrepareTask([renderPass] {renderPass->Prepare(s_RenderData); }, dependencies);
            };

		if (s_RaytracingScene)
			SchedulePrepareTask([] {s_RaytracingScene->Prepare(s_RenderData); }, {});
        SchedulePassPrepare(s_ShadowCasterPass, {});

		const std::shared_ptr<Worker::Task> gBufferPrepareTask = SchedulePassPrepare(s_GBufferPass, {});

        if (s_RaytracingPass)
        {
            const std::shared_ptr<Worker::Task> raytracedShadowsDependencies[1] = { gBufferPrepareTask };
            SchedulePassPrepare(s_RaytracingPass, raytracedShadowsDependencies);
        }

        const std::shared_ptr<Worker::Task> forwardRenderPassDependencies[1] = { gBufferPrepareTask };
        SchedulePassPrepare(s_ForwardRenderPass, forwardRenderPassDependencies);

		SchedulePassPrepare(s_SkyboxPass, {});

        const std::shared_ptr<Worker::Task> deferredLightPrepareTask = SchedulePassPrepare(s_DeferredLightPass, forwardRenderPassDependencies);

        const std::shared_ptr<Worker::Task> postProcessDependencies[1] = { deferredLightPrepareTask };
        SchedulePassPrepare(s_PostProcessPass, postProcessDependencies);

        const std::shared_ptr<Worker::Task> uiRenderPrepareTask = SchedulePassPrepare(s_UIRenderPass, {});
        SchedulePassPrepare(s_FinalBlitPass, {});

#if RENDER_ENGINE_EDITOR
        const std::shared_ptr<Worker::Task> gizmos3DPrepareTask = SchedulePassPrepare(s_3DGizmosPass, {});
        
        const std::shared_ptr<Worker::Task> gizmos2DDependencies[1] = { uiRenderPrepareTask };
        const std::shared_ptr<Worker::Task> gizmos2DPrepareTask = SchedulePassPrepare(s_2DGizmosPass, gizmos2DDependencies);

        const std::shared_ptr<Worker::Task> clearGizmosDependencies[2] = { gizmos2DPrepareTask, gizmos3DPrepareTask };
        SchedulePrepareTask([] {Gizmos::ClearGizmos(); }, clearGizmosDependencies);
        
        SchedulePassPrepare(s_SelectionOutlinePass, {});
        SchedulePassPrepare(s_ShadowMapDebugPass, {});
#endif

        if (s_SynchronousGraphicsPrepare)
            s_PrepareTask->Execute();
        else
			s_PrepareTask->Schedule();
    }

    void Render()
    {
        if (s_RenderData.Viewport.x == 0 || s_RenderData.Viewport.y == 0)
            return;

        s_PrepareTask->Wait();

        Profiler::Marker marker("Graphics::Render");

        GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
        GraphicsBackend::Current()->SetClearDepth(1);

        SetLightingData(s_RenderData.Lights, s_RenderData.Skybox);

        if (s_RaytracingScene)
	        s_RaytracingScene->Update(s_RenderData);

        s_ShadowCasterPass->Execute(s_RenderData);
		s_GBufferPass->Execute(s_RenderData);
        if (s_RaytracingPass)
			s_RaytracingPass->ExecuteRaytracedShadows(s_RenderData);
		s_DeferredLightPass->Execute(s_RenderData);
		s_SkyboxPass->Execute(s_RenderData);
        s_ForwardRenderPass->Execute(s_RenderData);
#if RENDER_ENGINE_EDITOR
        s_ShadowMapDebugPass->Execute(s_RenderData);
        s_3DGizmosPass->Execute(s_RenderData);
        s_SelectionOutlinePass->Execute(s_RenderData);
        if (s_RaytracingPass)
            s_RaytracingPass->ExecutePrimaryRaysDebug(s_RenderData);
#endif
        s_PostProcessPass->Execute(s_RenderData);
        s_UIRenderPass->Execute(s_RenderData);
#if RENDER_ENGINE_EDITOR
        s_2DGizmosPass->Execute(s_RenderData);
#endif
        s_FinalBlitPass->Execute(s_RenderData);

    }

    int GetScreenWidth()
    {
        return s_ScreenWidth;
    }

    int GetScreenHeight()
    {
        return s_ScreenHeight;
    }

    void SetCameraData(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, float nearPlane, float farPlane)
    {
        const Matrix4x4 gpuProjectionMatrix = GetGPUProjectionMatrix(projectionMatrix);
        const Matrix4x4 invViewMatrix = viewMatrix.Invert();

		Matrix4x4 depthRemap = Matrix4x4::Identity();
        depthRemap.m22 = 2.0f;
		depthRemap.m32 = -1.0f;

        CameraData cameraData{};
        cameraData.CameraPosition = invViewMatrix.GetPosition();
        cameraData.NearClipPlane = nearPlane;
        cameraData.FarClipPlane = farPlane;
        cameraData.ViewProjectionMatrix = gpuProjectionMatrix * viewMatrix;
		cameraData.InvViewProjectionMatrix = (projectionMatrix * viewMatrix).Invert() * depthRemap;
        cameraData.CameraDirection = invViewMatrix * Vector4{0, 0, 1, 0};

        uint64_t offset = s_CameraDataBuffer->SetData(&cameraData, 0, sizeof(cameraData));
        GraphicsBackend::Current()->BindConstantBuffer(s_CameraDataBuffer->GetBackendBuffer(), GlobalConstants::CameraDataIndex, offset, sizeof(cameraData));
    }

    Matrix4x4 GetGPUProjectionMatrix(const Matrix4x4& projectionMatrix)
    {
        Matrix4x4 gpuProjectionMatrix = projectionMatrix;

        if (GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL || GraphicsBackend::Current()->GetName() == GraphicsBackendName::DX12)
        {
            // Projection matrix has OpenGL depth range [-1, 1]. Remap it to [0, 1] for Metal and DX12
            Matrix4x4 depthRemap = Matrix4x4::Identity();
            depthRemap.m22 = 0.5f;
            depthRemap.m32 = 0.5f;
            gpuProjectionMatrix = depthRemap * gpuProjectionMatrix;
        }

        return gpuProjectionMatrix;
    }

    bool IsPrepareSynchronous()
    {
        return s_SynchronousGraphicsPrepare;
    }
} // namespace Graphics