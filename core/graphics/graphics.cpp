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

#include <cassert>

namespace Graphics
{
    std::shared_ptr<GraphicsBuffer> s_LightingDataBuffer;
    std::shared_ptr<RingBuffer> s_CameraDataBuffer;

    std::shared_ptr<ForwardRenderPass> s_ForwardRenderPass;
    std::shared_ptr<ShadowCasterPass> s_ShadowCasterPass;
    std::shared_ptr<FinalBlitPass> s_FinalBlitPass;
    std::shared_ptr<UIRenderPass> s_UIRenderPass;

#if RENDER_ENGINE_EDITOR
    std::shared_ptr<CopyDepthPass> s_CopyDepthPass;
    std::shared_ptr<GizmosPass> s_GizmosPass;
    std::shared_ptr<SelectionOutlinePass> s_SelectionOutlinePass;
    std::shared_ptr<ShadowMapDebugPass> s_ShadowMapDebugPass;
#endif

    int s_ScreenWidth  = 0;
    int s_ScreenHeight = 0;
    RenderData s_RenderData;
    std::vector<std::shared_ptr<RenderPass>> s_RenderPasses;

    void InitConstantBuffers()
    {
        GraphicsBackendBufferDescriptor descriptor{};
        descriptor.AllowCPUWrites = true;

        descriptor.Size = sizeof(CameraData) * 32;
        s_CameraDataBuffer = std::make_shared<RingBuffer>(descriptor, "CameraData");

        descriptor.Size = sizeof(LightingData);
        s_LightingDataBuffer = std::make_shared<GraphicsBuffer>(descriptor, "LightingData");
    }

    std::shared_ptr<Shader> s_ComputeShader;
    std::shared_ptr<Texture2D> s_TextureIn;
    std::shared_ptr<Texture2D> s_TextureOut;
    std::shared_ptr<GraphicsBuffer> s_TypedBufferIn;
    std::shared_ptr<GraphicsBuffer> s_TypedBufferOut;
    std::shared_ptr<GraphicsBuffer> s_StructuredBufferIn;
    std::shared_ptr<GraphicsBuffer> s_StructuredBufferOut;
    std::shared_ptr<GraphicsBuffer> s_ByteBufferIn;
    std::shared_ptr<GraphicsBuffer> s_ByteBufferOut;

    void InitPasses()
    {
        s_ShadowCasterPass = std::make_shared<ShadowCasterPass>(0);
        s_ForwardRenderPass = std::make_shared<ForwardRenderPass>(1);
        s_FinalBlitPass = std::make_shared<FinalBlitPass>(3);
        s_UIRenderPass = std::make_shared<UIRenderPass>(4);

#if RENDER_ENGINE_EDITOR
        s_CopyDepthPass = std::make_shared<CopyDepthPass>(2);
        s_GizmosPass = std::make_shared<GizmosPass>(6);
        s_SelectionOutlinePass = std::make_shared<SelectionOutlinePass>(5);
        s_ShadowMapDebugPass = std::make_shared<ShadowMapDebugPass>(7);
#endif

        s_ComputeShader = Shader::Load("core_resources/shaders/compute_test", {});

        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = 8;
        descriptor.Height = 8;
        descriptor.Linear = true;
        descriptor.Format = TextureInternalFormat::R32F;

        s_TextureIn = Texture2D::Create(descriptor, "ComputeTest/TextureIn");

        descriptor.ReadWrite = true;
        s_TextureOut = Texture2D::Create(descriptor, "ComputeTest/TextureOut");

        GraphicsBackendBufferDescriptor readOnlyBufferDescriptor{};
        readOnlyBufferDescriptor.AllowCPUWrites = true;

        GraphicsBackendBufferDescriptor rwBufferDescriptor{};
        rwBufferDescriptor.AllowGPUWrites = true;

        readOnlyBufferDescriptor.Size = 64 * sizeof(float);
        s_TypedBufferIn = std::make_shared<GraphicsBuffer>(readOnlyBufferDescriptor, "ComputeText/TypedBufferIn", false);

        rwBufferDescriptor.Size = 64 * sizeof(float);
        s_TypedBufferOut = std::make_shared<GraphicsBuffer>(rwBufferDescriptor, "ComputeText/TypedBufferOut", false);

        float data[64];
        for (int i = 0; i < 64; ++i)
            data[i] = i;
        s_TypedBufferIn->SetData(&data[0], 0, sizeof(data));

        struct TestStruct
        {
            float A;
            float B;
        };

        readOnlyBufferDescriptor.Size = 64 * sizeof(TestStruct);
        s_StructuredBufferIn = std::make_shared<GraphicsBuffer>(readOnlyBufferDescriptor, "ComputeTest/StructuredBufferIn", false);

        rwBufferDescriptor.Size = 64 * sizeof(TestStruct);
        s_StructuredBufferOut = std::make_shared<GraphicsBuffer>(rwBufferDescriptor, "ComputeTest/StructuredBufferOut", false);

        TestStruct structuredData[64];
        for (int i = 0; i < 64; ++i)
        {
            structuredData[i].A = i;
            structuredData[i].B = i * 2;
        }
        s_StructuredBufferIn->SetData(&structuredData[0], 0, 64 * sizeof(TestStruct));

        readOnlyBufferDescriptor.Size = 64 * sizeof(float);
        s_ByteBufferIn = std::make_shared<GraphicsBuffer>(readOnlyBufferDescriptor, "ComputeText/ByteBufferIn", false);

        rwBufferDescriptor.Size = 64 * sizeof(float);
        s_ByteBufferOut = std::make_shared<GraphicsBuffer>(rwBufferDescriptor, "ComputeText/ByteBufferOut", false);
        s_ByteBufferIn->SetData(&data[0], 0, sizeof(data));

        float pixels[64] = {1.0f};
        std::fill(&pixels[0], &pixels[64], 1.0f);
        GraphicsBackend::Current()->UploadImagePixels(s_TextureIn->GetBackendTexture(), 0, 8, 8, 0, 64 * sizeof(float), &pixels[0]);
    }

    void Init()
    {
#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitConstantBuffers();
        InitPasses();
    }

    void Shutdown()
    {
        s_LightingDataBuffer = nullptr;
        s_CameraDataBuffer = nullptr;

        s_ForwardRenderPass = nullptr;
        s_ShadowCasterPass = nullptr;
        s_FinalBlitPass = nullptr;
        s_UIRenderPass = nullptr;

#if RENDER_ENGINE_EDITOR
        s_CopyDepthPass = nullptr;
        s_GizmosPass = nullptr;
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

        static std::shared_ptr<Texture2D> cameraColorTarget;
        static std::shared_ptr<Texture2D> cameraDepthTarget;

        static GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
        static GraphicsBackendRenderTargetDescriptor depthTargetDescriptor{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

        s_RenderPasses.clear();
        s_RenderData = RenderData::GetRenderData(width, height);

        if (cameraColorTarget == nullptr || cameraColorTarget->GetWidth() != width || cameraColorTarget->GetHeight() != height)
        {
	        const TextureInternalFormat depthFormat = GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL ? TextureInternalFormat::DEPTH_32_STENCIL_8 : TextureInternalFormat::DEPTH_24_STENCIL_8;

	        GraphicsBackendTextureDescriptor descriptor;
	        descriptor.Width = width;
	        descriptor.Height = height;
	        descriptor.Linear = true;
	        descriptor.RenderTarget = true;

	        descriptor.Format = TextureInternalFormat::RGBA16F;
	        cameraColorTarget = Texture2D::Create(descriptor, "CameraColorRT");

	        descriptor.Format = depthFormat;
	        cameraDepthTarget = Texture2D::Create(descriptor, "CameraDepthRT");
        }

        colorTargetDescriptor.Texture = cameraColorTarget->GetBackendTexture();
        depthTargetDescriptor.Texture = cameraDepthTarget->GetBackendTexture();

        s_ShadowCasterPass->Prepare(s_RenderData);
        s_ForwardRenderPass->Prepare(s_RenderData, colorTargetDescriptor, depthTargetDescriptor);
        s_FinalBlitPass->Prepare(cameraColorTarget);
        s_UIRenderPass->Prepare(s_RenderData);

        s_RenderPasses.push_back(s_ShadowCasterPass);
        s_RenderPasses.push_back(s_ForwardRenderPass);
        s_RenderPasses.push_back(s_FinalBlitPass);
        s_RenderPasses.push_back(s_UIRenderPass);

#if RENDER_ENGINE_EDITOR
        const bool executeGizmosPass = s_GizmosPass->Prepare(s_RenderData, s_CopyDepthPass->GetEndFence());
        const bool executeSelectionPass = s_SelectionOutlinePass->Prepare(s_RenderData);

        if (executeGizmosPass)
        {
	        s_CopyDepthPass->Prepare(s_ForwardRenderPass->GetEndFence(), cameraDepthTarget);

	        s_RenderPasses.push_back(s_CopyDepthPass);
	        s_RenderPasses.push_back(s_GizmosPass);
        }

        if (executeSelectionPass)
            s_RenderPasses.push_back(s_SelectionOutlinePass);

        s_ShadowMapDebugPass->Prepare(cameraDepthTarget);
        s_RenderPasses.push_back(s_ShadowMapDebugPass);
#endif
    }

    void Render()
    {
        if (s_RenderData.Viewport.x == 0 || s_RenderData.Viewport.y == 0)
            return;

        Profiler::Marker marker("Graphics::Render");

        GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
        GraphicsBackend::Current()->SetClearDepth(1);

        SetLightingData(s_RenderData.Lights, s_RenderData.Skybox);

        std::sort(s_RenderPasses.begin(), s_RenderPasses.end(), RenderPass::Comparer());
        for (const std::shared_ptr<RenderPass>& pass : s_RenderPasses)
	        pass->Execute(s_RenderData);

        GraphicsBackend::Current()->BeginComputePass("Compute Test");

        GraphicsBackend::Current()->BindBuffer(s_TypedBufferIn->GetBackendBuffer(), 0, 0, 64, TextureInternalFormat::R32F);
        GraphicsBackend::Current()->BindRWBuffer(s_TypedBufferOut->GetBackendBuffer(), 0, 0, 64, TextureInternalFormat::R32F);

        GraphicsBackend::Current()->BindBuffer(s_StructuredBufferIn->GetBackendBuffer(), 1, 0, s_StructuredBufferIn->GetSize(), 64);
        GraphicsBackend::Current()->BindRWBuffer(s_StructuredBufferOut->GetBackendBuffer(), 1, 0, s_StructuredBufferOut->GetSize(), 64);

        GraphicsBackend::Current()->BindBuffer(s_ByteBufferIn->GetBackendBuffer(), 2, 0, s_ByteBufferIn->GetSize());
        GraphicsBackend::Current()->BindRWBuffer(s_ByteBufferOut->GetBackendBuffer(), 2, 0, s_ByteBufferOut->GetSize());

        GraphicsBackend::Current()->BindTexture(s_TextureIn->GetBackendTexture(), 0);
        GraphicsBackend::Current()->BindRWTexture(s_TextureOut->GetBackendTexture(), 0);

        GraphicsBackend::Current()->UseProgram(s_ComputeShader->GetProgram());
        GraphicsBackend::Current()->Dispatch(64, 1, 1);

        GraphicsBackend::Current()->EndComputePass();
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

        CameraData cameraData{};
        cameraData.CameraPosition = invViewMatrix.GetPosition();
        cameraData.NearClipPlane = nearPlane;
        cameraData.FarClipPlane = farPlane;
        cameraData.ViewProjectionMatrix = gpuProjectionMatrix * viewMatrix;
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
} // namespace Graphics