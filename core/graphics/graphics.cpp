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
#include "graphics_backend_debug.h"
#include "enums/cull_face_orientation.h"
#include "enums/graphics_backend_capability.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/indices_data_type.h"
#include "enums/framebuffer_target.h"
#include "types/graphics_backend_framebuffer.h"
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

#include <cassert>
#include <boost/functional/hash/hash.hpp>

namespace Graphics
{
    std::shared_ptr<GraphicsBuffer> instancingMatricesBuffer;
    std::shared_ptr<GraphicsBuffer> instancingDataBuffer;
    std::shared_ptr<GraphicsBuffer> lightingDataBlock;
    std::shared_ptr<GraphicsBuffer> cameraDataBlock;
    std::shared_ptr<GraphicsBuffer> shadowsDataBlock;

    int perDrawDataStride = 0;
    int currentPerDrawOffset = 0;
    uint64_t perDrawCapacity = 100;
    std::vector<std::shared_ptr<GraphicsBuffer>> perDrawDataBuffers;

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

    GraphicsBackendFramebuffer framebuffer;

    PropertyBlock globalPropertyBlock;

    void InitCulling()
    {
        GraphicsBackend::Current()->SetCullFaceOrientation(CullFaceOrientation::CLOCKWISE);
    }

    void InitDepth()
    {
        GraphicsBackend::Current()->SetCapability(GraphicsBackendCapability::DEPTH_TEST, true);
        GraphicsBackend::Current()->SetDepthWrite(true);
        GraphicsBackend::Current()->SetDepthRange(0, 1);
    }

    void AddPerDrawDataBuffer()
    {
        uint64_t maxSize = GraphicsBackend::Current()->GetMaxConstantBufferSize();
        int alignment = GraphicsBackend::Current()->GetConstantBufferOffsetAlignment();

        perDrawDataStride = sizeof(PerDrawData);
        if (perDrawDataStride % alignment != 0)
        {
            perDrawDataStride = (perDrawDataStride / alignment + 1) * alignment;
        }

        uint64_t bufferSize = perDrawDataStride * perDrawCapacity;
        if (bufferSize > maxSize)
        {
            perDrawCapacity = maxSize / perDrawDataStride;
            bufferSize = perDrawDataStride * perDrawCapacity;
        }

        auto buffer = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, bufferSize, BufferUsageHint::DYNAMIC_DRAW);
        perDrawDataBuffers.push_back(buffer);
    }

    void InitUniformBlocks()
    {
        // C++ struct memory layout must match GPU side struct
        assert(sizeof(CameraData) == 96);
        assert(sizeof(LightingData) == 288);
        assert(sizeof(ShadowsData) == 1456);
        assert(sizeof(PerDrawData) == 128);

        cameraDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(CameraData), BufferUsageHint::DYNAMIC_DRAW);
        lightingDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(LightingData), BufferUsageHint::DYNAMIC_DRAW);
        shadowsDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(ShadowsData), BufferUsageHint::DYNAMIC_DRAW);
        AddPerDrawDataBuffer();
    }

    void InitPasses()
    {
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque(), ClearMask::COLOR_DEPTH, "Forward");
        transparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent(), ClearMask::NONE, "Forward");
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();
        finalBlitPass        = std::make_unique<FinalBlitPass>();

#if RENDER_ENGINE_EDITOR
        fallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::All(), ClearMask::NONE, "Fallback");
        gizmosPass         = std::make_unique<GizmosPass>();
        selectionOutlinePass = std::make_unique<SelectionOutlinePass>();
#endif
    }

    void InitInstancing()
    {
        auto supportSSBO = GraphicsBackend::Current()->SupportShaderStorageBuffer();
        auto matricesBufferTarget = supportSSBO ? BufferBindTarget::SHADER_STORAGE_BUFFER : BufferBindTarget::ARRAY_BUFFER;
        auto dataBufferTarget = supportSSBO ? BufferBindTarget::SHADER_STORAGE_BUFFER : BufferBindTarget::UNIFORM_BUFFER;
        auto matricesBufferSize = sizeof(Matrix4x4) * GlobalConstants::MaxInstancingCount * 2;

        instancingMatricesBuffer = std::make_shared<GraphicsBuffer>(matricesBufferTarget, matricesBufferSize, BufferUsageHint::DYNAMIC_DRAW);
        instancingDataBuffer = std::make_shared<GraphicsBuffer>(dataBufferTarget, 1, BufferUsageHint::DYNAMIC_DRAW);
    }

    void InitSeamlessCubemap()
    {
        GraphicsBackend::Current()->SetCapability(GraphicsBackendCapability::TEXTURE_CUBE_MAP_SEAMLESS, true);
    }

    void Init()
    {
#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitCulling();
        InitDepth();
        InitUniformBlocks();
        InitPasses();
        InitInstancing();
        InitSeamlessCubemap();
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
        currentPerDrawOffset = 0;

        static std::shared_ptr<Texture2D> cameraColorTarget;
        static std::shared_ptr<Texture2D> cameraDepthTarget;

        screenWidth  = width;
        screenHeight = height;

        if (cameraColorTarget == nullptr || cameraColorTarget->GetWidth() != width || cameraColorTarget->GetHeight() != height)
        {
            cameraColorTarget = Texture2D::Create(width, height, TextureInternalFormat::RGBA16F, true);
            cameraDepthTarget = Texture2D::Create(width, height, TextureInternalFormat::DEPTH_COMPONENT, true);
        }

        auto debugGroup = GraphicsBackendDebug::DebugGroup("Render Frame");

        GraphicsBackend::Current()->SetClearColor(0, 0, 0, 0);
        GraphicsBackend::Current()->SetClearDepth(1);

        Context ctx;

        SetLightingData(ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        SetViewport({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)});
        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        SetRenderTargets(cameraColorTarget, 0, 0, cameraDepthTarget, 0, 0);

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
        if (finalBlitPass)
            finalBlitPass->Execute(ctx, cameraColorTarget);

        SetRenderTargets(nullptr, 0, 0, nullptr, 0, 0);

#if RENDER_ENGINE_EDITOR

        {
            auto copyDepthDebugGroup = GraphicsBackendDebug::DebugGroup("Copy Depth to BackBuffer");
            Blit(cameraDepthTarget, BlitFramebufferMask::DEPTH, BlitFramebufferFilter::NEAREST);
        }

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

        GraphicsBackendDebug::CheckError();
    }

    void SetupGeometry(const DrawableGeometry &geometry)
    {
        auto vao = geometry.GetVertexArrayObject();
        GraphicsBackend::Current()->BindVertexArrayObject(vao);
    }

    void SetupMatrices(const Matrix4x4 &modelMatrix)
    {
        ++currentPerDrawOffset;
        if (currentPerDrawOffset > perDrawCapacity)
        {
            AddPerDrawDataBuffer();
        }

        PerDrawData perDrawData{};
        perDrawData.ModelMatrix = modelMatrix;
        perDrawData.ModelNormalMatrix = modelMatrix.Invert().Transpose();
        perDrawDataBuffers[currentPerDrawOffset / perDrawCapacity]->SetData(&perDrawData, perDrawDataStride * currentPerDrawOffset, sizeof(perDrawData));
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

    void SetupInstancing(bool _enabled)
    {
        if (GraphicsBackend::Current()->SupportShaderStorageBuffer())
        {
            // matrices are supplied in SSBO if it is supported
            return;
        }

        int baseAttribute = GlobalConstants::InstancingBaseVertexAttribute;

        // make sure VAO is bound before calling this function
        if (_enabled)
        {
            // matrix4x4 requires 4 vertex attributes
            for (int i = 0; i < 8; ++i)
            {
                GraphicsBackend::Current()->EnableVertexAttributeArray(baseAttribute + i);
                GraphicsBackend::Current()->SetVertexAttributeDivisor(baseAttribute + i, 1);
            }

            auto vec4Size = sizeof(Vector4);

            // model matrix
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 0, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(0));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 1, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(1 * vec4Size));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 2, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(2 * vec4Size));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 3, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(3 * vec4Size));

            // normal matrix
            auto offset = sizeof(Matrix4x4);
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 4, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 5, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 1 * vec4Size));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 6, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 2 * vec4Size));
            GraphicsBackend::Current()->SetVertexAttributePointer(baseAttribute + 7, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 3 * vec4Size));
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                GraphicsBackend::Current()->DisableVertexAttributeArray(baseAttribute + i);
                GraphicsBackend::Current()->SetVertexAttributeDivisor(baseAttribute + i, 0);
            }
        }
    }

    void SetBlendState(const BlendInfo& blendInfo)
    {
        GraphicsBackend::Current()->SetCapability(GraphicsBackendCapability::BLEND, blendInfo.Enabled);

        if (blendInfo.Enabled)
        {
            GraphicsBackend::Current()->SetBlendFunction(blendInfo.SourceFactor, blendInfo.DestinationFactor);
        }
    }

    void SetCullState(const CullInfo &cullInfo)
    {
        GraphicsBackend::Current()->SetCapability(GraphicsBackendCapability::CULL_FACE, cullInfo.Enabled);

        if (cullInfo.Enabled)
        {
            GraphicsBackend::Current()->SetCullFace(cullInfo.Face);
        }
    }

    void SetDepthState(const DepthInfo &depthInfo)
    {
        GraphicsBackend::Current()->SetDepthWrite(depthInfo.WriteDepth);
        GraphicsBackend::Current()->SetDepthFunction(depthInfo.DepthFunction);
    }

    void SetUniform(const std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, const std::string &name, const void *data)
    {
        auto it = uniforms.find(name);
        if (it != uniforms.end())
        {
            auto &uniformInfo = it->second;
            GraphicsBackend::Current()->SetUniform(uniformInfo.Location, uniformInfo.Type, 1, data);
        }
    }

    void SetTextureUniform(const std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, const std::string &name, const Texture &texture)
    {
        auto it = uniforms.find(name);
        if (it == uniforms.end())
            return;

        const auto& uniformInfo = it->second;
        if (uniformInfo.IsTexture)
        {
            texture.Bind(uniformInfo.TextureBindings, uniformInfo.HasSampler, uniformInfo.Location);
        }
    }

    void SetPropertyBlock(const PropertyBlock &propertyBlock, const ShaderPass &shaderPass)
    {
        const auto &uniforms = shaderPass.GetUniforms();

        for (const auto &pair: propertyBlock.GetTextures())
        {
            if (pair.second != nullptr)
                SetTextureUniform(uniforms, pair.first, *pair.second);
        }
        for (const auto &pair: propertyBlock.GetVectors())
            SetUniform(uniforms, pair.first, &pair.second);
        for (const auto &pair: propertyBlock.GetFloats())
            SetUniform(uniforms, pair.first, &pair.second);
        for (const auto &pair: propertyBlock.GetMatrices())
            SetUniform(uniforms, pair.first, &pair.second);
        for (const auto &pair: propertyBlock.GetInts())
            SetUniform(uniforms, pair.first, &pair.second);
    }

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, int offset, int size, const ShaderPass &shaderPass)
    {
        if (!buffer)
        {
            return;
        }

        const auto &buffers = shaderPass.GetBuffers();

        auto it = buffers.find(name);
        if (it != buffers.end())
        {
            buffer->Bind(it->second->GetBinding(), offset, size);
        }
    }

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, const ShaderPass &shaderPass)
    {
        if (buffer)
        {
            SetGraphicsBuffer(name, buffer, 0, buffer->GetSize(), shaderPass);
        }
    }

    void SetupPerInstanceData(const std::vector<GraphicsBufferWrapper*> &instancesDataBuffers, int shaderPass)
    {
        auto debugGroup = GraphicsBackendDebug::DebugGroup("Setup PerInstanceData");

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

    void SetupShaderPass(int shaderPassIndex, const Material &material, const std::shared_ptr<GraphicsBuffer> &perInstanceDataBuffer)
    {
        const auto &shaderPass = *material.GetShader()->GetPass(shaderPassIndex);
        const auto &perMaterialDataBlock = material.GetPerMaterialDataBlock(shaderPassIndex);
        const auto &materialPropertyBlock = material.GetPropertyBlock();

        GraphicsBackend::Current()->UseProgram(shaderPass.GetProgram());

        SetBlendState(shaderPass.GetBlendInfo());
        SetCullState(shaderPass.GetCullInfo());
        SetDepthState(shaderPass.GetDepthInfo());

        SetGraphicsBuffer(GlobalConstants::LightingBufferName, lightingDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::CameraDataBufferName, cameraDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::ShadowsBufferName, shadowsDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerDrawDataBufferName, perDrawDataBuffers[currentPerDrawOffset / perDrawCapacity], perDrawDataStride * currentPerDrawOffset, sizeof(PerDrawData), shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerMaterialDataBufferName, perMaterialDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerInstanceDataBufferName, perInstanceDataBuffer, shaderPass);
        if (GraphicsBackend::Current()->SupportShaderStorageBuffer())
        {
            SetGraphicsBuffer(GlobalConstants::InstanceMatricesBufferName, instancingMatricesBuffer, shaderPass);
        }

        SetPropertyBlock(shaderPass.GetDefaultValuesBlock(), shaderPass);
        SetPropertyBlock(globalPropertyBlock, shaderPass);
        SetPropertyBlock(materialPropertyBlock, shaderPass);
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
            const auto &shader = drawCall.Material->GetShader();
            for (int i = 0; i < shader->PassesCount(); ++i)
            {
                auto pass = shader->GetPass(i);
                if (settings.TagsMatch(*pass))
                {
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
    }

    void Draw(const DrawableGeometry &geometry, const Material &material, const Matrix4x4 &modelMatrix, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData)
    {
        SetupMatrices(modelMatrix);
        SetupShaderPass(shaderPassIndex, material, perInstanceData);

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
        SetupInstancing(true);
        SetupShaderPass(shaderPassIndex, material, perInstanceData);

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

        SetupInstancing(false);
    }

    int GetScreenWidth()
    {
        return screenWidth;
    }

    int GetScreenHeight()
    {
        return screenHeight;
    }

    void SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix)
    {
        CameraData cameraData{};
        cameraData.CameraPosition = _viewMatrix.Invert().GetPosition();
        cameraData.NearClipPlane = Camera::Current->GetNearClipPlane();
        cameraData.FarClipPlane = Camera::Current->GetFarClipPlane();
        cameraData.ViewProjectionMatrix = _projectionMatrix * _viewMatrix;
        cameraData.CameraDirection = Camera::Current->GetRotation() * Vector3{0, 0, 1};

        cameraDataBlock->SetData(&cameraData, 0, sizeof(cameraData));

        lastCameraPosition = cameraData.CameraPosition;
    }

    const std::string &GetGlobalShaderDirectives()
    {
        // clang-format off
        static std::string globalShaderDirectives = GraphicsBackend::Current()->GetShadingLanguageDirective() + "\n"
                                                    "#define MAX_POINT_LIGHT_SOURCES " + std::to_string(GlobalConstants::MaxPointLightSources) + "\n"
                                                    "#define MAX_SPOT_LIGHT_SOURCES " + std::to_string(GlobalConstants::MaxSpotLightSources) + "\n"
                                                    "#define MAX_INSTANCING_COUNT " + std::to_string(GlobalConstants::MaxInstancingCount) + "\n"
                                                    "#define SUPPORT_SSBO " + std::to_string(GraphicsBackend::Current()->SupportShaderStorageBuffer() ? 1 : 0) + "\n";
        // clang-format on

        return globalShaderDirectives;
    }

    void SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, int colorLevel, int colorLayer,
                          const std::shared_ptr<Texture> &_depthAttachment, int depthLevel, int depthLayer)
    {
        if (!_colorAttachment && !_depthAttachment)
        {
            GraphicsBackend::Current()->AttachBackbuffer();
            return;
        }

        if (_colorAttachment)
        {
            _colorAttachment->Attach(FramebufferAttachment::COLOR_ATTACHMENT0, colorLevel, colorLayer);
        }
        else
        {
            GraphicsBackend::Current()->AttachTexture(FramebufferAttachment::COLOR_ATTACHMENT0, GraphicsBackendTexture::NONE, 0, 0);
        }

        if (_depthAttachment)
        {
            _depthAttachment->Attach(FramebufferAttachment::DEPTH_ATTACHMENT, depthLevel, depthLayer);
        }
        else
        {
            GraphicsBackend::Current()->AttachTexture(FramebufferAttachment::DEPTH_ATTACHMENT, GraphicsBackendTexture::NONE, 0, 0);
        }
    }

    void SetViewport(const Vector4 &viewport)
    {
        GraphicsBackend::Current()->SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    }

    void SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture> &texture)
    {
        globalPropertyBlock.SetTexture(name, texture);
    }

    void CopyBufferData(const std::shared_ptr<GraphicsBuffer> &source, const std::shared_ptr<GraphicsBuffer> &destination, int sourceOffset, int destinationOffset, int size)
    {
        if (!source || !destination)
        {
            return;
        }

        source->Bind(BufferBindTarget::COPY_READ_BUFFER);
        destination->Bind(BufferBindTarget::COPY_WRITE_BUFFER);
        GraphicsBackend::Current()->CopyBufferSubData(BufferBindTarget::COPY_READ_BUFFER, BufferBindTarget::COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size);
    }

    void Blit(const std::shared_ptr<Texture> &source, const std::shared_ptr<Texture> &destination, int destinationLevel, int destinationLayer, Material &material)
    {
        static std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();

        material.SetTexture("_BlitTexture", source);

        SetRenderTargets(destination, destinationLevel, destinationLayer, nullptr, 0, 0);
        Draw(*fullscreenMesh, material, Matrix4x4::Identity(), 0);
    }

    void Blit(const std::shared_ptr<Texture> &source, BlitFramebufferMask mask, BlitFramebufferFilter filter)
    {
        FramebufferAttachment attachment = FramebufferAttachment::COLOR_ATTACHMENT0;
        if (mask == BlitFramebufferMask::DEPTH)
        {
            attachment = FramebufferAttachment::DEPTH_ATTACHMENT;
        }
        else if (mask == BlitFramebufferMask::STENCIL)
        {
            attachment = FramebufferAttachment::STENCIL_ATTACHMENT;
        }
        else if (mask == BlitFramebufferMask::DEPTH_STENCIL)
        {
            attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
        }

        GraphicsBackend::Current()->BindFramebuffer(FramebufferTarget::READ_FRAMEBUFFER, framebuffer);
        source->Attach(attachment, 0, 0);

        GraphicsBackend::Current()->BindFramebuffer(FramebufferTarget::DRAW_FRAMEBUFFER, GraphicsBackendFramebuffer::NONE);
        GraphicsBackend::Current()->BlitFramebuffer(0, 0, source->GetWidth(), source->GetHeight(), 0, 0, screenWidth, screenHeight, mask, filter);
    }
} // namespace Graphics