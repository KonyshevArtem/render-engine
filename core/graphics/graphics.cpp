#include "graphics.h"
#include "camera/camera.h"
#include "context.h"
#include "debug.h"
#include "draw_call_info.h"
#include "editor/gizmos/gizmos.h"
#include "editor/gizmos/gizmos_pass.h"
#include "light/light.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug_group.h"
#include "enums/indices_data_type.h"
#include "shader/shader.h"
#include "data_structs/camera_data.h"
#include "data_structs/lighting_data.h"
#include "data_structs/per_draw_data.h"
#include "types/graphics_backend_buffer_info.h"
#include "render_settings/draw_call_filter.h"
#include "texture_2d/texture_2d.h"
#include "mesh/mesh.h"
#include "passes/final_blit_pass.h"
#include "editor/selection_outline/selection_outline_pass.h"
#include "graphics_settings.h"
#include "graphics_buffer/ring_buffer.h"
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

#include <cassert>

namespace Graphics
{
    std::shared_ptr<RingBuffer> s_InstancingMatricesBuffer;
    std::shared_ptr<GraphicsBuffer> s_LightingDataBuffer;
    std::shared_ptr<GraphicsBuffer> s_ShadowsDataBuffer;

    std::shared_ptr<RingBuffer> s_PerDrawDataBuffer;
    std::shared_ptr<RingBuffer> s_CameraDataBuffer;

    std::shared_ptr<ForwardRenderPass> s_ForwardRenderPass;
    std::shared_ptr<ShadowCasterPass> s_ShadowCasterPass;
    std::shared_ptr<FinalBlitPass> s_FinalBlitPass;

#if RENDER_ENGINE_EDITOR
    std::shared_ptr<CopyDepthPass> s_CopyDepthPass;
    std::shared_ptr<GizmosPass> s_GizmosPass;
    std::shared_ptr<SelectionOutlinePass> s_SelectionOutlinePass;
    std::shared_ptr<ShadowMapDebugPass> s_ShadowMapDebugPass;
#endif

    int s_ScreenWidth  = 0;
    int s_ScreenHeight = 0;

    std::unordered_map<std::string, std::shared_ptr<Texture>> s_GlobalTextures;

    void InitConstantBuffers()
    {
        // C++ struct memory layout must match GPU side struct
        assert(sizeof(CameraData) == 96);
        assert(sizeof(LightingData) == 304);
        assert(sizeof(ShadowsData) == 1456);
        assert(sizeof(PerDrawData) == 128);

        s_CameraDataBuffer = std::make_shared<RingBuffer>(sizeof(CameraData), 32, "CameraData");
        s_LightingDataBuffer = std::make_shared<GraphicsBuffer>(sizeof(LightingData), "LightingData");
        s_ShadowsDataBuffer = std::make_shared<GraphicsBuffer>(sizeof(ShadowsData), "ShadowsData");
        s_PerDrawDataBuffer = std::make_shared<RingBuffer>(sizeof(PerDrawData), 1024, "PerDrawData");
    }

    void InitPasses()
    {
        s_ShadowCasterPass = std::make_shared<ShadowCasterPass>(s_ShadowsDataBuffer, 0);
        s_ForwardRenderPass = std::make_shared<ForwardRenderPass>(1);
        s_FinalBlitPass = std::make_shared<FinalBlitPass>(3);

#if RENDER_ENGINE_EDITOR
        s_CopyDepthPass = std::make_shared<CopyDepthPass>(2);
        s_GizmosPass = std::make_shared<GizmosPass>(5);
        s_SelectionOutlinePass = std::make_shared<SelectionOutlinePass>(4);
        s_ShadowMapDebugPass = std::make_shared<ShadowMapDebugPass>(6);
#endif
    }

    void InitInstancing()
    {
        auto matricesBufferSize = sizeof(Matrix4x4) * GlobalConstants::MaxInstancingCount * 2;

        s_InstancingMatricesBuffer = std::make_shared<RingBuffer>(matricesBufferSize, 64, "PerInstanceMatrices");
    }

    void Init()
    {
#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitConstantBuffers();
        InitPasses();
        InitInstancing();
    }

    void Shutdown()
    {
    }

    void SetLightingData(const std::vector<Light*>& lights, const std::shared_ptr<Texture>& skybox)
    {
        LightingData lightingData{};
        lightingData.AmbientLight = GraphicsSettings::GetAmbientLightColor() * GraphicsSettings::GetAmbientLightIntensity();
        lightingData.PointLightsCount = 0;
        lightingData.SpotLightsCount = 0;
        lightingData.HasDirectionalLight = -1;
        lightingData.ReflectionCubeMips = skybox ? skybox->GetMipLevels() : 1;

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

        SetGlobalTexture("_ReflectionCube", skybox ? skybox : Cubemap::White());
        s_LightingDataBuffer->SetData(&lightingData, 0, sizeof(lightingData));
    }

    void Render(int width, int height)
    {
        if (width == 0 || height == 0)
            return;

        static std::shared_ptr<Texture2D> cameraColorTarget;
        static std::shared_ptr<Texture2D> cameraDepthTarget;

        static GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
        static GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

        s_ScreenWidth  = width;
        s_ScreenHeight = height;

        const Context ctx;
        std::vector<std::shared_ptr<RenderPass>> renderPasses;
        {
            Profiler::Marker marker("Prepare Render");

            if (cameraColorTarget == nullptr || cameraColorTarget->GetWidth() != width || cameraColorTarget->GetHeight() != height)
            {
                const TextureInternalFormat depthFormat = GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL ? TextureInternalFormat::DEPTH_32_STENCIL_8 : TextureInternalFormat::DEPTH_24_STENCIL_8;

                cameraColorTarget = Texture2D::Create(width, height, TextureInternalFormat::RGBA16F, true, true, "CameraColorRT");
                cameraDepthTarget = Texture2D::Create(width, height, depthFormat, true, true, "CameraDepthRT");
            }

            colorTargetDescriptor.Texture = cameraColorTarget->GetBackendTexture();
            depthTargetDescriptor.Texture = cameraDepthTarget->GetBackendTexture();

            GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
            GraphicsBackend::Current()->SetClearDepth(1);

            SetLightingData(ctx.Lights, ctx.Skybox);

            s_ShadowCasterPass->Prepare(ctx.Renderers, ctx.Lights, Camera::Current->GetShadowDistance());
            s_ForwardRenderPass->Prepare(colorTargetDescriptor, depthTargetDescriptor, Camera::Current->GetGameObject()->GetPosition(), ctx.Renderers);
            s_FinalBlitPass->Prepare(cameraColorTarget);

            renderPasses.push_back(s_ShadowCasterPass);
            renderPasses.push_back(s_ForwardRenderPass);
            renderPasses.push_back(s_FinalBlitPass);

#if RENDER_ENGINE_EDITOR
            const bool executeGizmosPass = s_GizmosPass->Prepare(ctx.Renderers, s_CopyDepthPass->GetEndFence());
            const bool executeSelectionPass = s_SelectionOutlinePass->Prepare();

            if (executeGizmosPass)
            {
                s_CopyDepthPass->Prepare(s_ForwardRenderPass->GetEndFence(), cameraDepthTarget);

                renderPasses.push_back(s_CopyDepthPass);
                renderPasses.push_back(s_GizmosPass);
            }

            if (executeSelectionPass)
                renderPasses.push_back(s_SelectionOutlinePass);

            renderPasses.push_back(s_ShadowMapDebugPass);
#endif
        }

        {
            Profiler::Marker marker("Execute Render");

            std::sort(renderPasses.begin(), renderPasses.end(), RenderPass::Comparer());
            for (const std::shared_ptr<RenderPass>& pass : renderPasses)
            {
                pass->Execute(ctx);
            }
        }
    }

    void SetupMatrices(const Matrix4x4 &modelMatrix)
    {
        PerDrawData perDrawData{};
        perDrawData.ModelMatrix = modelMatrix;
        perDrawData.ModelNormalMatrix = modelMatrix.Invert().Transpose();
        s_PerDrawDataBuffer->SetData(&perDrawData, 0, sizeof(perDrawData));
    }

    void SetupMatrices(const std::vector<Matrix4x4> &matrices)
    {
        static std::vector<Matrix4x4> matricesBuffer;

        auto count = matrices.size();

        matricesBuffer.resize(count * 2);
        for (int i = 0; i < count; ++i)
        {
            matricesBuffer[i * 2 + 0] = matrices[i];
            matricesBuffer[i * 2 + 1] = matrices[i].Invert().Transpose();
        }

        auto matricesSize = sizeof(Matrix4x4) * matricesBuffer.size();
        s_InstancingMatricesBuffer->SetData(matricesBuffer.data(), 0, matricesSize);
    }

    void SetTextures(const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, const Shader& shader)
    {
        const auto &shaderTextures = shader.GetTextures();
        const auto &shaderSamplers = shader.GetSamplers();

        for (const auto &pair: textures)
        {
            const std::string &texName = pair.first;
            const std::shared_ptr<Texture> &texture = pair.second;

            if (!texture)
                continue;

            auto texInfoIt = shaderTextures.find(texName);
            if (texInfoIt == shaderTextures.end())
                continue;

            const GraphicsBackendTextureInfo &textureInfo = texInfoIt->second;
            const std::string samplerName = "sampler" + texName;

            GraphicsBackend::Current()->BindTexture(textureInfo.TextureBindings, texture->GetBackendTexture());

            auto samplerInfoIt = shaderSamplers.find(samplerName);
            if (samplerInfoIt != shaderSamplers.end())
                GraphicsBackend::Current()->BindSampler(samplerInfoIt->second.Bindings, texture->GetBackendSampler());
        }
    }

    bool TryFindBufferInfo(const std::string &name, const Shader& shader, GraphicsBackendBufferInfo*& outInfo)
    {
        const auto &buffers = shader.GetBuffers();

        auto it = buffers.find(name);
        bool found = it != buffers.end();
        if (found)
            outInfo = it->second.get();
        return found;
    }

    void BindBuffer(const std::string& name, const GraphicsBackendBuffer& buffer, const Shader& shader, int offset, int size)
    {
        GraphicsBackendBufferInfo* info;
        if (TryFindBufferInfo(name, shader, info))
        {
            switch (info->GetBufferType())
            {
                case BufferType::RAW_BYTE_BUFFER:
                    GraphicsBackend::Current()->BindBuffer(buffer, info->GetBinding(), offset, size);
                case BufferType::STRUCTURED_BUFFER:
                    GraphicsBackend::Current()->BindStructuredBuffer(buffer, info->GetBinding(), offset / info->GetSize(), info->GetSize(), size / info->GetSize());
                case BufferType::CONSTANT_BUFFER:
                    GraphicsBackend::Current()->BindConstantBuffer(buffer, info->GetBinding(), offset, size);
            }
        }
    }

    void BindBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, const Shader& shader, int offset = 0)
    {
        if (buffer)
            BindBuffer(name, buffer->GetBackendBuffer(), shader, offset, buffer->GetSize());
    }

    void BindBuffer(const std::string &name, const std::shared_ptr<RingBuffer>& buffer, const Shader& shader, int offset = 0)
    {
        if (buffer)
            BindBuffer(name, buffer->GetBackendBuffer(), shader, buffer->GetCurrentElementOffset() + offset, buffer->GetElementSize());
    }

    void SetupShaderPass(const Material &material, const VertexAttributes &vertexAttributes, PrimitiveType primitiveType)
    {
        Shader& shaderPass = *material.GetShader();
        const auto &perMaterialDataBuffer = material.GetPerMaterialDataBuffer();

        bool isLinear;
        TextureInternalFormat colorTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::COLOR_ATTACHMENT0, &isLinear);
        TextureInternalFormat depthTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, nullptr);
        GraphicsBackend::Current()->UseProgram(shaderPass.GetProgram(vertexAttributes, colorTargetFormat, isLinear, depthTargetFormat, primitiveType));

        BindBuffer(GlobalConstants::LightingBufferName, s_LightingDataBuffer, shaderPass);
        BindBuffer(GlobalConstants::CameraDataBufferName, s_CameraDataBuffer, shaderPass);
        BindBuffer(GlobalConstants::ShadowsBufferName, s_ShadowsDataBuffer, shaderPass);
        BindBuffer(GlobalConstants::PerDrawDataBufferName, s_PerDrawDataBuffer, shaderPass);
        BindBuffer(GlobalConstants::PerMaterialDataBufferName, perMaterialDataBuffer, shaderPass);
        BindBuffer(GlobalConstants::InstanceMatricesBufferName, s_InstancingMatricesBuffer, shaderPass);

        SetTextures(s_GlobalTextures, shaderPass);
        SetTextures(material.GetTextures(), shaderPass);
    }

    void DrawRenderQueue(const RenderQueue& renderQueue)
    {
        for (const DrawCallInfo& drawCall : renderQueue.GetDrawCalls())
        {
            if (drawCall.Instanced)
                DrawInstanced(*drawCall.Geometry, *drawCall.Material, drawCall.ModelMatrices);
            else
                Draw(*drawCall.Geometry, *drawCall.Material, drawCall.ModelMatrices[0]);
        }
    }

    void Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix)
    {
        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();

        SetupMatrices(modelMatrix);
        SetupShaderPass(material, geometry.GetVertexAttributes(), geometry.GetPrimitiveType());

        if (geometry.HasIndexes())
        {
            GraphicsBackend::Current()->DrawElements(geometry.GetGraphicsBackendGeometry(), primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT);
        }
        else
        {
            GraphicsBackend::Current()->DrawArrays(geometry.GetGraphicsBackendGeometry(), primitiveType, 0, elementsCount);
        }
    }

    void DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices)
    {
        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();
        auto instanceCount = modelMatrices.size();

        SetupMatrices(modelMatrices);
        SetupShaderPass(material, geometry.GetVertexAttributes(), geometry.GetPrimitiveType());

        if (geometry.HasIndexes())
        {
            GraphicsBackend::Current()->DrawElementsInstanced(geometry.GetGraphicsBackendGeometry(), primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT, instanceCount);
        }
        else
        {
            GraphicsBackend::Current()->DrawArraysInstanced(geometry.GetGraphicsBackendGeometry(), primitiveType, 0, elementsCount, instanceCount);
        }
    }

    int GetScreenWidth()
    {
        return s_ScreenWidth;
    }

    int GetScreenHeight()
    {
        return s_ScreenHeight;
    }

    void SetCameraData(const Matrix4x4 &_viewMatrix, Matrix4x4 _projectionMatrix)
    {
        if (GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL || GraphicsBackend::Current()->GetName() == GraphicsBackendName::DX12)
        {
            // Projection matrix has OpenGL depth range [-1, 1]. Remap it to [0, 1] for Metal and DX12
            Matrix4x4 depthRemap = Matrix4x4::Identity();
            depthRemap.m22 = 0.5f;
            depthRemap.m32 = 0.5f;
            _projectionMatrix = depthRemap * _projectionMatrix;
        }

        CameraData cameraData{};
        cameraData.CameraPosition = _viewMatrix.Invert().GetPosition();
        cameraData.NearClipPlane = Camera::Current->GetNearClipPlane();
        cameraData.FarClipPlane = Camera::Current->GetFarClipPlane();
        cameraData.ViewProjectionMatrix = _projectionMatrix * _viewMatrix;
        cameraData.CameraDirection = Camera::Current->GetGameObject()->GetRotation() * Vector3{0, 0, 1};

        s_CameraDataBuffer->SetData(&cameraData, 0, sizeof(cameraData));
    }

    void SetRenderTarget(GraphicsBackendRenderTargetDescriptor descriptor, const std::shared_ptr<Texture> &target)
    {
        if (target)
        {
            descriptor.Texture = target->GetBackendTexture();
        }

        GraphicsBackend::Current()->AttachRenderTarget(descriptor);
    }

    void SetViewport(const Vector4 &viewport)
    {
        GraphicsBackend::Current()->SetViewport(viewport.x, viewport.y, viewport.z, viewport.w, 0, 1);
        GraphicsBackend::Current()->SetScissorRect(viewport.x, viewport.y, viewport.z, viewport.w);
    }

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture)
    {
        s_GlobalTextures[name] = texture;
    }

    void CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size)
    {
        if (source && destination)
        {
            GraphicsBackend::Current()->CopyBufferSubData(source->GetBackendBuffer(), destination->GetBackendBuffer(), sourceOffset, destinationOffset, size);
        }
    }

    void Blit(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, Material &material, const std::string& name)
    {
        static const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
        static const GraphicsBackendRenderTargetDescriptor depthDescriptor{FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT};

        material.SetTexture("_BlitTexture", source);

        SetRenderTarget(destinationDescriptor, destination);
        SetRenderTarget(depthDescriptor, nullptr);

        GraphicsBackend::Current()->BeginRenderPass(name);
        Draw(*fullscreenMesh, material, Matrix4x4::Identity());
        GraphicsBackend::Current()->EndRenderPass();
    }

    void CopyTextureToTexture(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, GraphicsBackendRenderTargetDescriptor destinationDescriptor)
    {
        if (!source)
            return;

        if (destination)
        {
            destinationDescriptor.Texture = destination->GetBackendTexture();
        }

        GraphicsBackend::Current()->CopyTextureToTexture(source->GetBackendTexture(), destinationDescriptor, 0, 0, 0, 0, source->GetWidth(), source->GetHeight());
    }
} // namespace Graphics