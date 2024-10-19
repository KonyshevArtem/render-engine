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
#include "passes/skybox_pass.h"
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics_backend_api.h"
#include "graphics_backend_debug_group.h"
#include "enums/indices_data_type.h"
#include "shader/shader.h"
#include "shader/shader_pass/shader_pass.h"
#include "data_structs/camera_data.h"
#include "data_structs/lighting_data.h"
#include "data_structs/per_draw_data.h"
#include "types/graphics_backend_buffer_info.h"
#include "render_settings/draw_call_comparer.h"
#include "render_settings/draw_call_filter.h"
#include "texture_2d/texture_2d.h"
#include "mesh/mesh.h"
#include "passes/final_blit_pass.h"
#include "editor/selection_outline/selection_outline_pass.h"
#include "graphics_settings.h"
#include "graphics_buffer/ring_buffer.h"
#include "types/graphics_backend_sampler_info.h"

#include <cassert>
#include <boost/functional/hash/hash.hpp>

namespace Graphics
{
    std::shared_ptr<RingBuffer> instancingMatricesBuffer;
    std::shared_ptr<GraphicsBuffer> instancingDataBuffer;
    std::shared_ptr<GraphicsBuffer> lightingDataBlock;
    std::shared_ptr<GraphicsBuffer> shadowsDataBlock;

    std::shared_ptr<RingBuffer> perDrawDataBuffer;
    std::shared_ptr<RingBuffer> cameraDataBuffer;

    std::unique_ptr<ShadowCasterPass> shadowCasterPass;
    std::unique_ptr<RenderPass>       opaqueRenderPass;
    std::unique_ptr<RenderPass>       transparentRenderPass;
    std::unique_ptr<SkyboxPass>       skyboxPass;
    std::unique_ptr<FinalBlitPass>    finalBlitPass;

#if RENDER_ENGINE_EDITOR
    std::unique_ptr<RenderPass> fallbackRenderPass;
    std::unique_ptr<GizmosPass> gizmosPass;
    std::unique_ptr<SelectionOutlinePass> selectionOutlinePass;
#endif

    int screenWidth  = 0;
    int screenHeight = 0;

    Vector3 lastCameraPosition;

    std::unordered_map<std::string, std::shared_ptr<Texture>> globalTextures;

    void InitUniformBlocks()
    {
        // C++ struct memory layout must match GPU side struct
        assert(sizeof(CameraData) == 96);
        assert(sizeof(LightingData) == 288);
        assert(sizeof(ShadowsData) == 1456);
        assert(sizeof(PerDrawData) == 128);

        cameraDataBuffer = std::make_shared<RingBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(CameraData), BufferUsageHint::DYNAMIC_DRAW);
        lightingDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(LightingData), BufferUsageHint::DYNAMIC_DRAW);
        shadowsDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(ShadowsData), BufferUsageHint::DYNAMIC_DRAW);
        perDrawDataBuffer = std::make_shared<RingBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(PerDrawData), BufferUsageHint::DYNAMIC_DRAW);
    }

    void InitPasses()
    {
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque(), "Forward");
        transparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent(), "Forward");
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();
        finalBlitPass        = std::make_unique<FinalBlitPass>();

#if RENDER_ENGINE_EDITOR
        fallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::All(), "Fallback");
        gizmosPass         = std::make_unique<GizmosPass>();
        selectionOutlinePass = std::make_unique<SelectionOutlinePass>();
#endif
    }

    void InitInstancing()
    {
        auto matricesBufferSize = sizeof(Matrix4x4) * GlobalConstants::MaxInstancingCount * 2;

        instancingMatricesBuffer = std::make_shared<RingBuffer>(BufferBindTarget::SHADER_STORAGE_BUFFER, matricesBufferSize, BufferUsageHint::DYNAMIC_DRAW);
        instancingDataBuffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::SHADER_STORAGE_BUFFER, 1, BufferUsageHint::DYNAMIC_DRAW);
    }

    void Init()
    {
#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitUniformBlocks();
        InitPasses();
        InitInstancing();
    }

    void Shutdown()
    {
    }

    void SetLightingData(const std::vector<Light *> &_lights)
    {
        LightingData lightingData{};
        lightingData.AmbientLight = GraphicsSettings::GetAmbientLightColor() * GraphicsSettings::GetAmbientLightIntensity();
        lightingData.PointLightsCount = 0;
        lightingData.SpotLightsCount = 0;
        lightingData.HasDirectionalLight = -1;

        for (const auto &light: _lights)
        {
            if (light == nullptr)
                continue;

            if (light->Type == LightType::DIRECTIONAL && lightingData.HasDirectionalLight <= 0)
            {
                lightingData.HasDirectionalLight = 1;
                lightingData.DirLightDirection = light->Rotation * Vector3(0, 0, 1);
                lightingData.DirLightIntensity = GraphicsSettings::GetSunLightColor() * GraphicsSettings::GetSunLightIntensity();
            }
            else if (light->Type == LightType::POINT && lightingData.PointLightsCount < GlobalConstants::MaxPointLightSources)
            {
                lightingData.PointLightsData[lightingData.PointLightsCount].Position = light->Position.ToVector4(1);
                lightingData.PointLightsData[lightingData.PointLightsCount].Intensity = light->Intensity;
                lightingData.PointLightsData[lightingData.PointLightsCount].Attenuation = light->Attenuation;
                ++lightingData.PointLightsCount;
            }
            else if (light->Type == LightType::SPOT && lightingData.SpotLightsCount < GlobalConstants::MaxSpotLightSources)
            {
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Position = light->Position.ToVector4(1);
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Direction = light->Rotation * Vector3(0, 0, 1);
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Intensity = light->Intensity;
                lightingData.SpotLightsData[lightingData.SpotLightsCount].Attenuation = light->Attenuation;
                lightingData.SpotLightsData[lightingData.SpotLightsCount].CutOffCosine = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
                ++lightingData.SpotLightsCount;
            }
        }

        lightingDataBlock->SetData(&lightingData, 0, sizeof(lightingData));
    }

    std::size_t GetDrawCallInstancingHash(const DrawCallInfo &drawCallInfo)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, drawCallInfo.Material);
        boost::hash_combine(seed, drawCallInfo.Geometry);
        return seed;
    }

    void BatchDrawCalls(std::vector<DrawCallInfo> &drawCalls, std::vector<std::vector<Matrix4x4>> &instancesMatrices, std::vector<std::vector<GraphicsBufferWrapper*>> &instancesDataBuffers)
    {
        static std::unordered_map<std::size_t, int> instancingMap;

        instancingMap.clear();
        instancesMatrices.clear();
        instancesDataBuffers.clear();

        for (int i = 0; i < drawCalls.size(); ++i)
        {
            auto &drawCall = drawCalls[i];
            if (!drawCall.Material->GetShader()->SupportInstancing())
            {
                continue;
            }

            auto hash = GetDrawCallInstancingHash(drawCall);
            auto it = instancingMap.find(hash);

            if (it == instancingMap.end())
            {
                int instancesDataIndex = instancesMatrices.size();
                instancesMatrices.push_back({drawCall.ModelMatrix});
                instancesDataBuffers.push_back({drawCall.InstanceDataBuffer});
                drawCall.InstancesDataIndex = instancesDataIndex;
                drawCall.Instanced = true;
                instancingMap[hash] = i;
                continue;
            }

            auto &instancedDrawCall = drawCalls[it->second];
            int instancedDataIndex = instancedDrawCall.InstancesDataIndex;
            instancesMatrices[instancedDataIndex].push_back(drawCall.ModelMatrix);
            instancesDataBuffers[instancedDataIndex].push_back(drawCall.InstanceDataBuffer);
            instancedDrawCall.AABB = instancedDrawCall.AABB.Combine(drawCall.AABB);

            drawCalls[i] = drawCalls[drawCalls.size() - 1];
            drawCalls.pop_back();
            --i;

            if (instancesMatrices[instancedDataIndex].size() >= GlobalConstants::MaxInstancingCount)
                instancingMap.erase(it);
        }
    }

    void Render(int width, int height)
    {
        instancingMatricesBuffer->Reset();
        perDrawDataBuffer->Reset();
        cameraDataBuffer->Reset();

        static std::shared_ptr<Texture2D> cameraColorTarget;
        static std::shared_ptr<Texture2D> cameraDepthTarget;

        static GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
        static GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

        screenWidth  = width;
        screenHeight = height;

        if (cameraColorTarget == nullptr || cameraColorTarget->GetWidth() != width || cameraColorTarget->GetHeight() != height)
        {
            cameraColorTarget = Texture2D::Create(width, height, TextureInternalFormat::RGBA16F, true);
            cameraDepthTarget = Texture2D::Create(width, height, TextureInternalFormat::DEPTH_COMPONENT, true);
        }

        GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
        GraphicsBackend::Current()->SetClearDepth(1);

        Context ctx;

        SetLightingData(ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        SetRenderTarget(colorTargetDescriptor, cameraColorTarget);
        SetRenderTarget(depthTargetDescriptor, cameraDepthTarget);
        GraphicsBackend::Current()->BeginRenderPass();

        SetViewport({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)});

        if (opaqueRenderPass)
            opaqueRenderPass->Execute(ctx);

#if RENDER_ENGINE_EDITOR
        if (fallbackRenderPass)
            fallbackRenderPass->Execute(ctx);
#endif

        if (skyboxPass)
            skyboxPass->Execute(ctx);
        if (transparentRenderPass)
            transparentRenderPass->Execute(ctx);

        GraphicsBackend::Current()->EndRenderPass();

        if (finalBlitPass)
            finalBlitPass->Execute(ctx, cameraColorTarget);

        SetRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
        SetRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

#if RENDER_ENGINE_EDITOR

        CopyTextureToTexture(cameraDepthTarget, nullptr, GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

        if (gizmosPass)
        {
            for (const auto &renderer : ctx.Renderers)
            {
                if (renderer)
                {
                    auto bounds = renderer->GetAABB();
                    Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
                }
            }
            gizmosPass->Execute(ctx);
        }

        if (selectionOutlinePass)
            selectionOutlinePass->Execute(ctx);

        Gizmos::ClearGizmos();
#endif
    }

    void SetupMatrices(const Matrix4x4 &modelMatrix)
    {
        PerDrawData perDrawData{};
        perDrawData.ModelMatrix = modelMatrix;
        perDrawData.ModelNormalMatrix = modelMatrix.Invert().Transpose();
        perDrawDataBuffer->SetData(&perDrawData, 0, sizeof(perDrawData));
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

        auto matricesSize = sizeof(Matrix4x4) * matricesBuffer.size() * 2;
        instancingMatricesBuffer->SetData(matricesBuffer.data(), 0, matricesSize);
    }

    void SetCullState(const CullInfo &cullInfo)
    {
        GraphicsBackend::Current()->SetCullFace(cullInfo.Face);
        GraphicsBackend::Current()->SetCullFaceOrientation(cullInfo.Orientation);
    }

    void SetTextures(const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, const ShaderPass &shaderPass)
    {
        const auto &shaderTextures = shaderPass.GetTextures();
        const auto &shaderSamplers = shaderPass.GetSamplers();

        for (const auto &pair: textures)
        {
            const auto &texName = pair.first;
            const auto &texture = pair.second;

            if (!texture)
                continue;

            auto texInfoIt = shaderTextures.find(texName);
            if (texInfoIt == shaderTextures.end())
                continue;

            const auto& textureInfo = texInfoIt->second;
            const auto samplerName = "sampler" + texName;

            auto samplerInfoIt = shaderSamplers.find(samplerName);
            if (samplerInfoIt == shaderSamplers.end())
                texture->Bind(textureInfo.TextureBindings, textureInfo.HasSampler);
            else
                texture->Bind(textureInfo.TextureBindings, &samplerInfoIt->second.Bindings);
        }
    }

    bool TryFindBufferBindings(const std::string &name, const ShaderPass &shaderPass, GraphicsBackendResourceBindings& outBindings)
    {
        const auto &buffers = shaderPass.GetBuffers();

        auto it = buffers.find(name);
        bool found = it != buffers.end();
        if (found)
        {
            outBindings = it->second->GetBinding();
        }

        return found;
    }

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, const ShaderPass &shaderPass)
    {
        GraphicsBackendResourceBindings bindings;
        if (buffer && TryFindBufferBindings(name, shaderPass, bindings))
        {
            buffer->Bind(bindings, 0, buffer->GetSize());
        }
    }

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<RingBuffer> &buffer, const ShaderPass &shaderPass)
    {
        GraphicsBackendResourceBindings bindings;
        if (buffer && TryFindBufferBindings(name, shaderPass, bindings))
        {
            buffer->Bind(bindings, 0, buffer->GetSize());
        }
    }

    void SetupPerInstanceData(const std::vector<GraphicsBufferWrapper*> &instancesDataBuffers, int shaderPass)
    {
        auto debugGroup = GraphicsBackendDebugGroup("Setup PerInstanceData");

        int instancesCount = instancesDataBuffers.size();
        for (int i = 0; i < instancesCount; ++i)
        {
            auto bufferWrapper = instancesDataBuffers[i];
            if (bufferWrapper)
            {
                auto &buffer = bufferWrapper->GetBuffer(shaderPass);
                if (buffer && buffer->GetSize() > 0)
                {
                    auto dataSize = buffer->GetSize();
                    if (instancingDataBuffer->GetSize() < dataSize * instancesCount)
                    {
                        instancingDataBuffer->Resize(dataSize * instancesCount);
                    }
                    CopyBufferData(buffer, instancingDataBuffer, 0, i * dataSize, dataSize);
                }
            }
        }
    }

    void SetupShaderPass(int shaderPassIndex, const Material &material, const std::shared_ptr<GraphicsBuffer> &perInstanceDataBuffer, const VertexAttributes &vertexAttributes)
    {
        auto &shaderPass = *material.GetShader()->GetPass(shaderPassIndex);
        const auto &perMaterialDataBlock = material.GetPerMaterialDataBlock(shaderPassIndex);

        TextureInternalFormat colorTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::COLOR_ATTACHMENT0);
        TextureInternalFormat depthTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_ATTACHMENT);
        GraphicsBackend::Current()->UseProgram(shaderPass.GetProgram(vertexAttributes, colorTargetFormat, depthTargetFormat));

        SetCullState(shaderPass.GetCullInfo());
        GraphicsBackend::Current()->SetDepthStencilState(shaderPass.GetDepthStencilState());

        SetGraphicsBuffer(GlobalConstants::LightingBufferName, lightingDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::CameraDataBufferName, cameraDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::ShadowsBufferName, shadowsDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerDrawDataBufferName, perDrawDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerMaterialDataBufferName, perMaterialDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerInstanceDataBufferName, perInstanceDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::InstanceMatricesBufferName, instancingMatricesBuffer, shaderPass);

        SetTextures(globalTextures, shaderPass);
        SetTextures(material.GetTextures(), shaderPass);
    }

    void SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>> &renderers, std::vector<DrawCallInfo> &drawCalls)
    {
        drawCalls.clear();
        drawCalls.reserve(renderers.size());

        for (const auto &renderer : renderers)
        {
            if (renderer)
            {
                auto geometry = renderer->GetGeometry().get();
                auto material = renderer->GetMaterial().get();
                if (geometry && material)
                {
                    drawCalls.push_back({geometry, material, renderer->GetModelMatrix(), renderer->GetAABB(),
                                         renderer->GetInstanceDataBufferWrapper().get(), renderer->CastShadows});
                }
            }
        }
    }

    void FilterDrawCalls(std::vector<DrawCallInfo> &drawCalls, const DrawCallFilter &filter)
    {
        for (int i = 0; i < drawCalls.size(); ++i)
        {
            const auto &drawCall = drawCalls[i];
            if (!filter(drawCall))
            {
                drawCalls[i] = drawCalls[drawCalls.size() - 1];
                drawCalls.pop_back();
                --i;
            }
        }
    }

    void SortDrawCalls(std::vector<DrawCallInfo> &drawCalls, DrawCallSortMode sortMode)
    {
        if (sortMode != DrawCallSortMode::NO_SORTING)
            std::sort(drawCalls.begin(), drawCalls.end(), DrawCallComparer {sortMode, lastCameraPosition});
    }

    void DrawRenderers(const std::vector<std::shared_ptr<Renderer>> &renderers, const RenderSettings &settings)
    {
        static std::vector<DrawCallInfo> filteredSortedDrawCalls;
        static std::vector<std::vector<Matrix4x4>> instancedMatrices;
        static std::vector<std::vector<GraphicsBufferWrapper*>> instancedDataBuffers;

        SetupDrawCalls(renderers, filteredSortedDrawCalls);
        FilterDrawCalls(filteredSortedDrawCalls, settings.Filter);
        BatchDrawCalls(filteredSortedDrawCalls, instancedMatrices, instancedDataBuffers);
        SortDrawCalls(filteredSortedDrawCalls, settings.Sorting);

        for (const auto &drawCall: filteredSortedDrawCalls)
        {
            const auto &shader = settings.OverrideMaterial ? settings.OverrideMaterial->GetShader() : drawCall.Material->GetShader();
            for (int i = 0; i < shader->PassesCount(); ++i)
            {
                auto pass = shader->GetPass(i);
                if (drawCall.Instanced)
                {
                    SetupPerInstanceData(instancedDataBuffers[drawCall.InstancesDataIndex], i);
                    DrawInstanced(*drawCall.Geometry, *drawCall.Material, instancedMatrices[drawCall.InstancesDataIndex], i, instancingDataBuffer);
                }
                else
                {
                    auto &instanceDataBuffer = drawCall.InstanceDataBuffer ? drawCall.InstanceDataBuffer->GetBuffer(i) : nullptr;
                    Draw(*drawCall.Geometry, *drawCall.Material, drawCall.ModelMatrix, i, instanceDataBuffer);
                }
            }
        }
    }

    void Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData)
    {
        SetupMatrices(modelMatrix);
        SetupShaderPass(shaderPassIndex, material, perInstanceData, geometry.GetVertexAttributes());

        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();

        if (geometry.HasIndexes())
        {
            GraphicsBackend::Current()->DrawElements(geometry.GetGraphicsBackendGeometry(), primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT);
        }
        else
        {
            GraphicsBackend::Current()->DrawArrays(geometry.GetGraphicsBackendGeometry(), primitiveType, 0, elementsCount);
        }
    }

    void DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData)
    {
        SetupMatrices(modelMatrices);
        SetupShaderPass(shaderPassIndex, material, perInstanceData, geometry.GetVertexAttributes());

        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();
        auto instanceCount = modelMatrices.size();

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
        return screenWidth;
    }

    int GetScreenHeight()
    {
        return screenHeight;
    }

    void SetCameraData(const Matrix4x4 &_viewMatrix, Matrix4x4 _projectionMatrix)
    {
        if (GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL)
        {
            // Projection matrix has OpenGL depth range [-1, 1]. Remap it to [0, 1] for Metal
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
        cameraData.CameraDirection = Camera::Current->GetRotation() * Vector3{0, 0, 1};

        cameraDataBuffer->SetData(&cameraData, 0, sizeof(cameraData));

        lastCameraPosition = cameraData.CameraPosition;
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
    }

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture)
    {
        globalTextures[name] = texture;
    }

    void CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size)
    {
        if (source && destination)
        {
            GraphicsBackend::Current()->CopyBufferSubData(source->GetBackendBuffer(), destination->GetBackendBuffer(), sourceOffset, destinationOffset, size);
        }
    }

    void Blit(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, const GraphicsBackendRenderTargetDescriptor& destinationDescriptor, Material &material)
    {
        static std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();

        material.SetTexture("_BlitTexture", source);

        SetRenderTarget(destinationDescriptor, destination);

        GraphicsBackend::Current()->BeginRenderPass();
        Draw(*fullscreenMesh, material, Matrix4x4::Identity(), 0);
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