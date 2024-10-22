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
#include "types/graphics_backend_render_target_descriptor.h"
#include "material/material.h"

#include <cassert>
#include <boost/functional/hash/hash.hpp>

namespace Graphics
{
    std::shared_ptr<RingBuffer> s_InstancingMatricesBuffer;
    std::shared_ptr<GraphicsBuffer> s_PerInstanceIndicesBuffer;
    std::shared_ptr<GraphicsBuffer> s_LightingDataBuffer;
    std::shared_ptr<GraphicsBuffer> s_ShadowsDataBuffer;

    std::shared_ptr<RingBuffer> s_PerDrawDataBuffer;
    std::shared_ptr<RingBuffer> s_CameraDataBuffer;

    std::unique_ptr<ShadowCasterPass> s_ShadowCasterPass;
    std::unique_ptr<RenderPass>       s_OpaqueRenderPass;
    std::unique_ptr<RenderPass>       s_TransparentRenderPass;
    std::unique_ptr<SkyboxPass>       s_SkyboxPass;
    std::unique_ptr<FinalBlitPass>    s_FinalBlitPass;

#if RENDER_ENGINE_EDITOR
    std::unique_ptr<RenderPass> s_FallbackRenderPass;
    std::unique_ptr<GizmosPass> s_GizmosPass;
    std::unique_ptr<SelectionOutlinePass> s_SelectionOutlinePass;
#endif

    int s_ScreenWidth  = 0;
    int s_ScreenHeight = 0;

    Vector3 s_LastCameraPosition;
    uint64_t s_PerInstanceIndicesOffset;

    std::unordered_map<std::string, std::shared_ptr<Texture>> s_GlobalTextures;

    void InitConstantBuffers()
    {
        // C++ struct memory layout must match GPU side struct
        assert(sizeof(CameraData) == 96);
        assert(sizeof(LightingData) == 288);
        assert(sizeof(ShadowsData) == 1456);
        assert(sizeof(PerDrawData) == 128);

        s_CameraDataBuffer = std::make_shared<RingBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(CameraData), BufferUsageHint::DYNAMIC_DRAW);
        s_LightingDataBuffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(LightingData), BufferUsageHint::DYNAMIC_DRAW);
        s_ShadowsDataBuffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(ShadowsData), BufferUsageHint::DYNAMIC_DRAW);
        s_PerDrawDataBuffer = std::make_shared<RingBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(PerDrawData), BufferUsageHint::DYNAMIC_DRAW);
    }

    void InitPasses()
    {
        s_OpaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque(), "Forward");
        s_TransparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent(), "Forward");
        s_ShadowCasterPass     = std::make_unique<ShadowCasterPass>(s_ShadowsDataBuffer);
        s_SkyboxPass           = std::make_unique<SkyboxPass>();
        s_FinalBlitPass        = std::make_unique<FinalBlitPass>();

#if RENDER_ENGINE_EDITOR
        s_FallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::All(), "Fallback");
        s_GizmosPass         = std::make_unique<GizmosPass>();
        s_SelectionOutlinePass = std::make_unique<SelectionOutlinePass>();
#endif
    }

    void InitInstancing()
    {
        auto matricesBufferSize = sizeof(Matrix4x4) * GlobalConstants::MaxInstancingCount * 2;

        s_InstancingMatricesBuffer = std::make_shared<RingBuffer>(BufferBindTarget::SHADER_STORAGE_BUFFER, matricesBufferSize, BufferUsageHint::DYNAMIC_DRAW);
        s_PerInstanceIndicesBuffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::SHADER_STORAGE_BUFFER, 1024, BufferUsageHint::DYNAMIC_DRAW);
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

        s_LightingDataBuffer->SetData(&lightingData, 0, sizeof(lightingData));
    }

    std::size_t GetDrawCallInstancingHash(const DrawCallInfo &drawCallInfo)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, drawCallInfo.Material);
        boost::hash_combine(seed, drawCallInfo.Geometry);
        return seed;
    }

    void BatchDrawCalls(std::vector<DrawCallInfo> &drawCalls, std::vector<std::vector<Matrix4x4>> &instancesMatrices, std::vector<std::vector<uint32_t>> &perInstanceIndices)
    {
        static std::unordered_map<std::size_t, size_t> instancingMap;

        instancingMap.clear();
        instancesMatrices.clear();
        perInstanceIndices.clear();

        for (size_t i = 0; i < drawCalls.size(); ++i)
        {
            DrawCallInfo& drawCall = drawCalls[i];
            if (!drawCall.Material->GetShader()->SupportInstancing())
            {
                continue;
            }

            size_t hash = GetDrawCallInstancingHash(drawCall);
            auto it = instancingMap.find(hash);

            if (it == instancingMap.end())
            {
                size_t dataIndex = instancesMatrices.size();
                instancesMatrices.push_back({drawCall.ModelMatrix});
                perInstanceIndices.push_back({drawCall.PerInstanceDataIndex});
                drawCall.InstancesDataIndex = dataIndex;
                drawCall.Instanced = true;
                instancingMap[hash] = i;
                continue;
            }

            DrawCallInfo& instancedDrawCall = drawCalls[it->second];
            size_t dataIndex = instancedDrawCall.InstancesDataIndex;
            instancesMatrices[dataIndex].push_back(drawCall.ModelMatrix);
            perInstanceIndices[dataIndex].push_back(drawCall.PerInstanceDataIndex);
            instancedDrawCall.AABB = instancedDrawCall.AABB.Combine(drawCall.AABB);

            drawCalls[i] = drawCalls[drawCalls.size() - 1];
            drawCalls.pop_back();
            --i;

            if (instancesMatrices[dataIndex].size() >= GlobalConstants::MaxInstancingCount)
                instancingMap.erase(it);
        }
    }

    void Render(int width, int height)
    {
        s_PerInstanceIndicesOffset = 0;
        s_InstancingMatricesBuffer->Reset();
        s_PerDrawDataBuffer->Reset();
        s_CameraDataBuffer->Reset();

        static std::shared_ptr<Texture2D> cameraColorTarget;
        static std::shared_ptr<Texture2D> cameraDepthTarget;

        static GraphicsBackendRenderTargetDescriptor colorTargetDescriptor { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = LoadAction::CLEAR };
        static GraphicsBackendRenderTargetDescriptor depthTargetDescriptor { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = LoadAction::CLEAR };

        s_ScreenWidth  = width;
        s_ScreenHeight = height;

        if (cameraColorTarget == nullptr || cameraColorTarget->GetWidth() != width || cameraColorTarget->GetHeight() != height)
        {
            cameraColorTarget = Texture2D::Create(width, height, TextureInternalFormat::RGBA16F, true);
            cameraDepthTarget = Texture2D::Create(width, height, TextureInternalFormat::DEPTH_COMPONENT, true);
        }

        GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
        GraphicsBackend::Current()->SetClearDepth(1);

        Context ctx;

        SetLightingData(ctx.Lights);

        if (s_ShadowCasterPass)
            s_ShadowCasterPass->Execute(ctx);

        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        SetRenderTarget(colorTargetDescriptor, cameraColorTarget);
        SetRenderTarget(depthTargetDescriptor, cameraDepthTarget);
        GraphicsBackend::Current()->BeginRenderPass();

        SetViewport({0, 0, static_cast<float>(s_ScreenWidth), static_cast<float>(s_ScreenHeight)});

        if (s_OpaqueRenderPass)
            s_OpaqueRenderPass->Execute(ctx);

#if RENDER_ENGINE_EDITOR
        if (s_FallbackRenderPass)
            s_FallbackRenderPass->Execute(ctx);
#endif

        if (s_SkyboxPass)
            s_SkyboxPass->Execute(ctx);
        if (s_TransparentRenderPass)
            s_TransparentRenderPass->Execute(ctx);

        GraphicsBackend::Current()->EndRenderPass();

        if (s_FinalBlitPass)
            s_FinalBlitPass->Execute(ctx, cameraColorTarget);

        SetRenderTarget(GraphicsBackendRenderTargetDescriptor::ColorBackbuffer());
        SetRenderTarget(GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

#if RENDER_ENGINE_EDITOR

        CopyTextureToTexture(cameraDepthTarget, nullptr, GraphicsBackendRenderTargetDescriptor::DepthBackbuffer());

        if (s_GizmosPass)
        {
            for (const auto &renderer : ctx.Renderers)
            {
                if (renderer)
                {
                    auto bounds = renderer->GetAABB();
                    Gizmos::DrawWireCube(Matrix4x4::TRS(bounds.GetCenter(), Quaternion(), bounds.GetSize() * 0.5f));
                }
            }
            s_GizmosPass->Execute(ctx);
        }

        if (s_SelectionOutlinePass)
            s_SelectionOutlinePass->Execute(ctx);

        Gizmos::ClearGizmos();
#endif
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

        auto matricesSize = sizeof(Matrix4x4) * matricesBuffer.size() * 2;
        s_InstancingMatricesBuffer->SetData(matricesBuffer.data(), 0, matricesSize);
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

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, const ShaderPass &shaderPass, int offset = 0)
    {
        GraphicsBackendResourceBindings bindings;
        if (buffer && TryFindBufferBindings(name, shaderPass, bindings))
        {
            buffer->Bind(bindings, offset, buffer->GetSize());
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

    void SetupPerInstanceIndices(const std::vector<uint32_t> &indicesBuffer, uint64_t& outBindOffset)
    {
        auto debugGroup = GraphicsBackendDebugGroup("Setup PerInstanceIndices");

        uint64_t size = indicesBuffer.size() * sizeof(int);
        uint64_t requiredSize = size + s_PerInstanceIndicesOffset;
        assert(s_PerInstanceIndicesBuffer->GetSize() >= requiredSize);

        s_PerInstanceIndicesBuffer->SetData(indicesBuffer.data(), s_PerInstanceIndicesOffset, size);

        outBindOffset = s_PerInstanceIndicesOffset;
        s_PerInstanceIndicesOffset = requiredSize;
    }

    void SetupShaderPass(int shaderPassIndex, const Material &material, const VertexAttributes &vertexAttributes,
                         const std::shared_ptr<GraphicsBuffer> &perInstanceData = nullptr, uint64_t perInstanceDataOffset = 0,
                         const std::shared_ptr<GraphicsBuffer> &perInstanceIndices = nullptr, uint64_t perInstanceIndicesOffset = 0)
    {
        auto &shaderPass = *material.GetShader()->GetPass(shaderPassIndex);
        const auto &perMaterialDataBuffer = material.GetPerMaterialDataBuffer(shaderPassIndex);

        TextureInternalFormat colorTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::COLOR_ATTACHMENT0);
        TextureInternalFormat depthTargetFormat = GraphicsBackend::Current()->GetRenderTargetFormat(FramebufferAttachment::DEPTH_ATTACHMENT);
        GraphicsBackend::Current()->UseProgram(shaderPass.GetProgram(vertexAttributes, colorTargetFormat, depthTargetFormat));

        SetCullState(shaderPass.GetCullInfo());
        GraphicsBackend::Current()->SetDepthStencilState(shaderPass.GetDepthStencilState());

        SetGraphicsBuffer(GlobalConstants::LightingBufferName, s_LightingDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::CameraDataBufferName, s_CameraDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::ShadowsBufferName, s_ShadowsDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerDrawDataBufferName, s_PerDrawDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerMaterialDataBufferName, perMaterialDataBuffer, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerInstanceDataBufferName, perInstanceData, shaderPass, perInstanceDataOffset);
        SetGraphicsBuffer(GlobalConstants::PerInstanceIndicesBufferName, perInstanceIndices, shaderPass, perInstanceIndicesOffset);
        SetGraphicsBuffer(GlobalConstants::InstanceMatricesBufferName, s_InstancingMatricesBuffer, shaderPass);

        SetTextures(s_GlobalTextures, shaderPass);
        SetTextures(material.GetTextures(), shaderPass);
    }

    void SetupDrawCalls(const std::vector<std::shared_ptr<Renderer>> &renderers, std::vector<DrawCallInfo> &drawCalls, const RenderSettings& settings)
    {
        drawCalls.clear();
        drawCalls.reserve(renderers.size());

        for (const auto &renderer : renderers)
        {
            if (renderer)
            {
                auto geometry = renderer->GetGeometry().get();
                auto material = settings.OverrideMaterial ? settings.OverrideMaterial.get() : renderer->GetMaterial().get();
                if (geometry && material)
                {
                    drawCalls.push_back({geometry, material, renderer->GetModelMatrix(), renderer->GetAABB(),
                                         renderer->GetInstanceDataIndex(), renderer->GetInstanceDataOffset(), renderer->CastShadows});
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
            std::sort(drawCalls.begin(), drawCalls.end(), DrawCallComparer {sortMode, s_LastCameraPosition});
    }

    void DrawRenderers(const std::vector<std::shared_ptr<Renderer>> &renderers, const RenderSettings &settings)
    {
        static std::vector<DrawCallInfo> filteredSortedDrawCalls;
        static std::vector<std::vector<Matrix4x4>> instancedMatrices;
        static std::vector<std::vector<uint32_t>> perInstanceIndices;

        SetupDrawCalls(renderers, filteredSortedDrawCalls, settings);
        FilterDrawCalls(filteredSortedDrawCalls, settings.Filter);
        BatchDrawCalls(filteredSortedDrawCalls, instancedMatrices, perInstanceIndices);
        SortDrawCalls(filteredSortedDrawCalls, settings.Sorting);

        for (const auto &drawCall: filteredSortedDrawCalls)
        {
            const auto &shader = drawCall.Material->GetShader();
            for (int i = 0; i < shader->PassesCount(); ++i)
            {
                auto pass = shader->GetPass(i);
                if (drawCall.Instanced)
                {
                    uint64_t indicesOffset;
                    SetupPerInstanceIndices(perInstanceIndices[drawCall.InstancesDataIndex], indicesOffset);

                    DrawInstanced(*drawCall.Geometry, *drawCall.Material, instancedMatrices[drawCall.InstancesDataIndex], i,
                        Renderer::GetInstanceDataBuffer(), 0,
                        s_PerInstanceIndicesBuffer, indicesOffset);
                }
                else
                {
                    Draw(*drawCall.Geometry, *drawCall.Material, drawCall.ModelMatrix, i, Renderer::GetInstanceDataBuffer(), drawCall.PerInstanceDataOffset);
                }
            }
        }
    }

    void Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix, int shaderPassIndex,
        const std::shared_ptr<GraphicsBuffer> &perInstanceData, uint64_t perInstanceDataOffset)
    {
        SetupMatrices(modelMatrix);
        SetupShaderPass(shaderPassIndex, material, geometry.GetVertexAttributes(), perInstanceData, perInstanceDataOffset);

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

    void DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices, int shaderPassIndex,
        const std::shared_ptr<GraphicsBuffer> &perInstanceData, uint64_t perInstanceDataOffset,
        const std::shared_ptr<GraphicsBuffer> &perInstanceIndices, uint64_t perInstanceIndicesOffset)
    {
        SetupMatrices(modelMatrices);
        SetupShaderPass(shaderPassIndex, material, geometry.GetVertexAttributes(),
            perInstanceData, perInstanceDataOffset, perInstanceIndices, perInstanceIndicesOffset);

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
        return s_ScreenWidth;
    }

    int GetScreenHeight()
    {
        return s_ScreenHeight;
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

        s_CameraDataBuffer->SetData(&cameraData, 0, sizeof(cameraData));

        s_LastCameraPosition = cameraData.CameraPosition;
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
        s_GlobalTextures[name] = texture;
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