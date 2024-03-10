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
#include "shader/uniform_info/buffer_info.h"
#include "render_settings/draw_call_comparer.h"
#include "render_settings/draw_call_filter.h"

#include <cassert>
#include <boost/functional/hash/hash.hpp>

namespace Graphics
{
    std::shared_ptr<GraphicsBuffer> instancingMatricesBuffer;
    std::shared_ptr<GraphicsBuffer> instancingDataBuffer;
    std::shared_ptr<GraphicsBuffer> lightingDataBlock;
    std::shared_ptr<GraphicsBuffer> cameraDataBlock;
    std::shared_ptr<GraphicsBuffer> shadowsDataBlock;
    std::shared_ptr<GraphicsBuffer> perDrawDataBlock;

    std::unique_ptr<ShadowCasterPass> shadowCasterPass;
    std::unique_ptr<RenderPass>       opaqueRenderPass;
    std::unique_ptr<RenderPass>       transparentRenderPass;
    std::unique_ptr<SkyboxPass>       skyboxPass;

#if RENDER_ENGINE_EDITOR
    std::unique_ptr<RenderPass> fallbackRenderPass;
    std::unique_ptr<GizmosPass> gizmosPass;
#endif

    int screenWidth  = 0;
    int screenHeight = 0;

    Vector3 lastCameraPosition;

    GraphicsBackendFramebuffer framebuffer;

    PropertyBlock globalPropertyBlock;

    void InitCulling()
    {
        GraphicsBackend::SetCullFaceOrientation(CullFaceOrientation::CLOCKWISE);
    }

    void InitDepth()
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::DEPTH_TEST, true);
        GraphicsBackend::SetDepthWrite(true);
        GraphicsBackend::SetDepthRange(0, 1);
    }

    void InitFramebuffer()
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::FRAMEBUFFER_SRGB, true);
        GraphicsBackend::GenerateFramebuffers(1, &framebuffer);
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
        perDrawDataBlock = std::make_shared<GraphicsBuffer>(BufferBindTarget::UNIFORM_BUFFER, sizeof(PerDrawData), BufferUsageHint::DYNAMIC_DRAW);
    }

    void InitPasses()
    {
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::Opaque(), ClearMask::COLOR_DEPTH, "Forward");
        transparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallSortMode::BACK_TO_FRONT, DrawCallFilter::Transparent(), ClearMask::NONE, "Forward");
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();

#if RENDER_ENGINE_EDITOR
        fallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallSortMode::FRONT_TO_BACK, DrawCallFilter::All(), ClearMask::NONE, "Fallback");
        gizmosPass         = std::make_unique<GizmosPass>();
#endif
    }

    void InitInstancing()
    {
        auto supportSSBO = GraphicsBackend::SupportShaderStorageBuffer();
        auto matricesBufferTarget = supportSSBO ? BufferBindTarget::SHADER_STORAGE_BUFFER : BufferBindTarget::ARRAY_BUFFER;
        auto dataBufferTarget = supportSSBO ? BufferBindTarget::SHADER_STORAGE_BUFFER : BufferBindTarget::UNIFORM_BUFFER;
        auto matricesBufferSize = sizeof(Matrix4x4) * GlobalConstants::MaxInstancingCount * 2;

        instancingMatricesBuffer = std::make_shared<GraphicsBuffer>(matricesBufferTarget, matricesBufferSize, BufferUsageHint::DYNAMIC_DRAW);
        instancingDataBuffer = std::make_shared<GraphicsBuffer>(dataBufferTarget, 0, BufferUsageHint::DYNAMIC_DRAW);
    }

    void InitSeamlessCubemap()
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::TEXTURE_CUBE_MAP_SEAMLESS, true);
    }

    void Init()
    {
        GraphicsBackend::Init();

#if RENDER_ENGINE_EDITOR
        Gizmos::Init();
#endif

        InitCulling();
        InitDepth();
        InitFramebuffer();
        InitUniformBlocks();
        InitPasses();
        InitInstancing();
        InitSeamlessCubemap();
    }

    void Shutdown()
    {
        GraphicsBackend::DeleteFramebuffers(1, &framebuffer);
    }

    void SetLightingData(const Vector3 &_ambient, const std::vector<Light *> &_lights)
    {
        LightingData lightingData{};
        lightingData.AmbientLight = _ambient;
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
                lightingData.DirLightIntensity = light->Intensity;
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
        screenWidth  = width;
        screenHeight = height;

        auto debugGroup = GraphicsBackendDebug::DebugGroup("Render Frame");

        GraphicsBackend::SetClearColor(0, 0, 0, 0);
        GraphicsBackend::SetClearDepth(1);

        Context ctx;

        SetLightingData(ctx.AmbientLight, ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        SetViewport({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)});
        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        if (opaqueRenderPass)
            opaqueRenderPass->Execute(ctx);
        if (skyboxPass)
            skyboxPass->Execute(ctx);
        if (transparentRenderPass)
            transparentRenderPass->Execute(ctx);

#if RENDER_ENGINE_EDITOR
        if (fallbackRenderPass)
            fallbackRenderPass->Execute(ctx);

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

        Gizmos::ClearGizmos();
#endif

        GraphicsBackendDebug::CheckError();
    }

    void SetupGeometry(const DrawableGeometry &geometry)
    {
        auto vao = geometry.GetVertexArrayObject();
        GraphicsBackend::BindVertexArrayObject(vao);
    }

    void SetupMatrices(const Matrix4x4 &modelMatrix)
    {
        PerDrawData perDrawData{};
        perDrawData.ModelMatrix = modelMatrix;
        perDrawData.ModelNormalMatrix = modelMatrix.Invert().Transpose();
        perDrawDataBlock->SetData(&perDrawData, 0, sizeof(perDrawData));
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
        if (GraphicsBackend::SupportShaderStorageBuffer())
        {
            // matrices are supplied in SSBO if it is supported
            return;
        }

        int baseAttribute = GlobalConstants::InstancingBaseVertexAttribute;

        // make sure VAO is bound before calling this function
        if (_enabled)
        {
            instancingMatricesBuffer->Bind();

            // matrix4x4 requires 4 vertex attributes
            for (int i = 0; i < 8; ++i)
            {
                GraphicsBackend::EnableVertexAttributeArray(baseAttribute + i);
                GraphicsBackend::SetVertexAttributeDivisor(baseAttribute + i, 1);
            }

            auto vec4Size = sizeof(Vector4);

            // model matrix
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 0, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(0));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 1, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(1 * vec4Size));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 2, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(2 * vec4Size));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 3, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(3 * vec4Size));

            // normal matrix
            auto offset = sizeof(Matrix4x4);
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 4, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 5, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 1 * vec4Size));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 6, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 2 * vec4Size));
            GraphicsBackend::SetVertexAttributePointer(baseAttribute + 7, 4, VertexAttributeDataType::FLOAT, false, 8 * vec4Size, reinterpret_cast<void *>(offset + 3 * vec4Size));

            GraphicsBackend::BindBuffer(BufferBindTarget::ARRAY_BUFFER, GraphicsBackendBuffer::NONE);
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                GraphicsBackend::DisableVertexAttributeArray(baseAttribute + i);
                GraphicsBackend::SetVertexAttributeDivisor(baseAttribute + i, 0);
            }
        }
    }

    void SetBlendState(const BlendInfo& blendInfo)
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::BLEND, blendInfo.Enabled);

        if (blendInfo.Enabled)
        {
            GraphicsBackend::SetBlendFunction(blendInfo.SourceFactor, blendInfo.DestinationFactor);
        }
    }

    void SetCullState(const CullInfo &cullInfo)
    {
        GraphicsBackend::SetCapability(GraphicsBackendCapability::CULL_FACE, cullInfo.Enabled);

        if (cullInfo.Enabled)
        {
            GraphicsBackend::SetCullFace(cullInfo.Face);
        }
    }

    void SetDepthState(const DepthInfo &depthInfo)
    {
        GraphicsBackend::SetDepthWrite(depthInfo.WriteDepth);
        GraphicsBackend::SetDepthFunction(depthInfo.DepthFunction);
    }

    void SetUniform(const std::unordered_map<std::string, UniformInfo> &uniforms, const std::string &name, const void *data)
    {
        auto it = uniforms.find(name);
        if (it != uniforms.end())
        {
            auto &uniformInfo = it->second;
            GraphicsBackend::SetUniform(uniformInfo.Location, uniformInfo.Type, 1, data);
        }
    }

    void SetTextureUniform(const std::unordered_map<std::string, UniformInfo> &uniforms, const std::string &name, const Texture &texture)
    {
        auto it = uniforms.find(name);
        if (it == uniforms.end())
            return;

        const auto& uniformInfo = it->second;
        if (!uniformInfo.IsTexture)
            return;

        texture.Bind(uniformInfo.TextureUnit);

        auto unitIndex = TextureUnitUtils::TextureUnitToIndex(uniformInfo.TextureUnit);
        SetUniform(uniforms, name, &unitIndex);
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
    }

    void SetGraphicsBuffer(const std::string &name, const std::shared_ptr<GraphicsBuffer> &buffer, const ShaderPass &shaderPass)
    {
        if (!buffer)
        {
            return;
        }

        const auto &buffers = shaderPass.GetBuffers();

        auto it = buffers.find(name);
        if (it != buffers.end())
        {
            buffer->Bind(it->second->GetBinding());
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

        GraphicsBackend::UseProgram(shaderPass.GetProgram());

        SetBlendState(shaderPass.GetBlendInfo());
        SetCullState(shaderPass.GetCullInfo());
        SetDepthState(shaderPass.GetDepthInfo());

        SetGraphicsBuffer(GlobalConstants::LightingBufferName, lightingDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::CameraDataBufferName, cameraDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::ShadowsBufferName, shadowsDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerDrawDataBufferName, perDrawDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerMaterialDataBufferName, perMaterialDataBlock, shaderPass);
        SetGraphicsBuffer(GlobalConstants::PerInstanceDataBufferName, perInstanceDataBuffer, shaderPass);
        if (GraphicsBackend::SupportShaderStorageBuffer())
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
        SetupGeometry(geometry);
        SetupMatrices(modelMatrix);
        SetupShaderPass(shaderPassIndex, material, perInstanceData);

        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();

        if (geometry.HasIndexes())
        {
            GraphicsBackend::DrawElements(primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT, nullptr);
        }
        else
        {
            GraphicsBackend::DrawArrays(primitiveType, 0, elementsCount);
        }
    }

    void DrawInstanced(const DrawableGeometry &geometry, const Material &material, const std::vector<Matrix4x4> &modelMatrices, int shaderPassIndex, const std::shared_ptr<GraphicsBuffer> &perInstanceData)
    {
        SetupGeometry(geometry);
        SetupMatrices(modelMatrices);
        SetupInstancing(true);
        SetupShaderPass(shaderPassIndex, material, perInstanceData);

        auto primitiveType = geometry.GetPrimitiveType();
        auto elementsCount = geometry.GetElementsCount();
        auto instanceCount = modelMatrices.size();

        if (geometry.HasIndexes())
        {
            GraphicsBackend::DrawElementsInstanced(primitiveType, elementsCount, IndicesDataType::UNSIGNED_INT, nullptr, instanceCount);
        }
        else
        {
            GraphicsBackend::DrawArraysInstanced(primitiveType, 0, elementsCount, instanceCount);
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
        static std::string globalShaderDirectives = GraphicsBackend::GetShadingLanguageDirective() + "\n"
                                                    "#define MAX_POINT_LIGHT_SOURCES " + std::to_string(GlobalConstants::MaxPointLightSources) + "\n"
                                                    "#define MAX_SPOT_LIGHT_SOURCES " + std::to_string(GlobalConstants::MaxSpotLightSources) + "\n"
                                                    "#define MAX_INSTANCING_COUNT " + std::to_string(GlobalConstants::MaxInstancingCount) + "\n"
                                                    "#define SUPPORT_SSBO " + std::to_string(GraphicsBackend::SupportShaderStorageBuffer() ? 1 : 0) + "\n";
        // clang-format on

        return globalShaderDirectives;
    }

    void SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, int colorLevel, int colorLayer,
                          const std::shared_ptr<Texture> &_depthAttachment, int depthLevel, int depthLayer)
    {
        if (!_colorAttachment && !_depthAttachment)
        {
            GraphicsBackend::BindFramebuffer(FramebufferTarget::DRAW_FRAMEBUFFER, GraphicsBackendFramebuffer::NONE);
            return;
        }

        GraphicsBackend::BindFramebuffer(FramebufferTarget::DRAW_FRAMEBUFFER, framebuffer);

        if (_colorAttachment)
            _colorAttachment->Attach(FramebufferAttachment::COLOR_ATTACHMENT0, colorLevel, colorLayer);
        else
        {
            GraphicsBackend::SetFramebufferTexture(FramebufferTarget::DRAW_FRAMEBUFFER, FramebufferAttachment::COLOR_ATTACHMENT0, GraphicsBackendTexture::NONE, 0);
        }

        if (_depthAttachment)
            _depthAttachment->Attach(FramebufferAttachment::DEPTH_ATTACHMENT, depthLevel, depthLayer);
        else
        {
            GraphicsBackend::SetFramebufferTexture(FramebufferTarget::DRAW_FRAMEBUFFER, FramebufferAttachment::DEPTH_ATTACHMENT, GraphicsBackendTexture::NONE, 0);
        }
    }

    void SetViewport(const Vector4 &viewport)
    {
        GraphicsBackend::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);
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
        GraphicsBackend::CopyBufferSubData(BufferBindTarget::COPY_READ_BUFFER, BufferBindTarget::COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size);
    }

} // namespace Graphics