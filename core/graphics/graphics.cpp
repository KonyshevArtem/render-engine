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
#include "render_settings.h"
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "uniform_block.h"
#include <boost/functional/hash/hash.hpp>
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

namespace Graphics
{
    struct DrawCallInfoHash
    {
        std::size_t operator()(const std::pair<Material *, DrawableGeometry *> &_pair) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, _pair.first);
            boost::hash_combine(seed, _pair.second);
            return seed;
        }
    };

    constexpr int MAX_POINT_LIGHT_SOURCES       = 3;
    constexpr int MAX_INSTANCING_COUNT          = 256;
    constexpr int INSTANCING_BASE_VERTEX_ATTRIB = 4;

    std::unique_ptr<UniformBlock> lightingDataBlock;
    std::unique_ptr<UniformBlock> cameraDataBlock;
    std::shared_ptr<UniformBlock> shadowsDataBlock;

    std::unique_ptr<ShadowCasterPass> shadowCasterPass;
    std::unique_ptr<RenderPass>       opaqueRenderPass;
    std::unique_ptr<RenderPass>       transparentRenderPass;
    std::unique_ptr<SkyboxPass>       skyboxPass;

#if OPENGL_STUDY_EDITOR
    std::unique_ptr<RenderPass> fallbackRenderPass;
    std::unique_ptr<GizmosPass> gizmosPass;
#endif

    int screenWidth  = 0;
    int screenHeight = 0;

    Vector3 lastCameraPosition;

    GLuint instancingMatricesBuffer;
    GLuint           framebuffer;

    void InitCulling()
    {
        CHECK_GL(glFrontFace(GL_CW))
    }

    void InitDepth()
    {
        CHECK_GL(glEnable(GL_DEPTH_TEST))
        CHECK_GL(glDepthMask(GL_TRUE))
        CHECK_GL(glDepthRange(0, 1))
    }

    void InitFramebuffer()
    {
        CHECK_GL(glEnable(GL_FRAMEBUFFER_SRGB))
        CHECK_GL(glGenFramebuffers(1, &framebuffer))
    }

    void InitUniformBlocks()
    {
        auto fullShader   = Shader::Load("resources/shaders/standard/standard.shader", {"_RECEIVE_SHADOWS"});
        cameraDataBlock   = std::make_unique<UniformBlock>(*fullShader, "CameraData", 0);
        lightingDataBlock = std::make_unique<UniformBlock>(*fullShader, "Lighting", 1);
        shadowsDataBlock  = std::make_shared<UniformBlock>(*fullShader, "Shadows", 2);

        cameraDataBlock->Bind();
        lightingDataBlock->Bind();
        shadowsDataBlock->Bind();
    }

    void InitPasses()
    {
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallInfo::Sorting::FRONT_TO_BACK, DrawCallInfo::Filter::Opaque(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, "Forward");
        transparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallInfo::Sorting::BACK_TO_FRONT, DrawCallInfo::Filter::Transparent(), 0, "Forward");
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();

#if OPENGL_STUDY_EDITOR
        fallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallInfo::Sorting::FRONT_TO_BACK, DrawCallInfo::Filter::All(), 0, "Fallback");
        gizmosPass         = std::make_unique<GizmosPass>();
#endif
    }

    void InitInstancing()
    {
        CHECK_GL(glGenBuffers(1, &instancingMatricesBuffer))
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer))
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4x4) * MAX_INSTANCING_COUNT * 2, nullptr, GL_STATIC_DRAW))
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0))
    }

    void InitSeamlessCubemap()
    {
        CHECK_GL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS))
    }

    void Init()
    {
#ifdef OPENGL_STUDY_WINDOWS
        auto result = glewInit();
        if (result != GLEW_OK)
            throw;
#endif

#if OPENGL_STUDY_EDITOR
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
        CHECK_GL(glDeleteBuffers(1, &instancingMatricesBuffer))
        CHECK_GL(glDeleteFramebuffers(1, &framebuffer))
    }

    void SetLightingData(const Vector3 &_ambient, const std::vector<Light *> &_lights)
    {
        static const std::string ambientLightName      = "_AmbientLight";
        static const std::string dirLightDirectionName = "_DirectionalLight.DirectionWS";
        static const std::string dirLightIntensityName = "_DirectionalLight.Intensity";
        static const std::string hasDirLightName       = "_DirectionalLight.HasDirectionalLight";

        static const std::string pointLightsCountName = "_PointLightsCount";
        static const std::string spotLightsCountName  = "_SpotLightsCount";

        static bool        namesInited = false;
        static std::string pointLightNames[MAX_POINT_LIGHT_SOURCES][3];
        static std::string spotLightNames[MAX_SPOT_LIGHT_SOURCES][5];

        if (!namesInited)
        {
            for (int i = 0; i < MAX_POINT_LIGHT_SOURCES; ++i)
            {
                auto prefix           = "_PointLights[" + std::to_string(i) + "].";
                pointLightNames[i][0] = prefix + "PositionWS";
                pointLightNames[i][1] = prefix + "Intensity";
                pointLightNames[i][2] = prefix + "Attenuation";
            }

            for (int i = 0; i < MAX_SPOT_LIGHT_SOURCES; ++i)
            {
                auto prefix          = "_SpotLights[" + std::to_string(i) + "].";
                spotLightNames[i][0] = prefix + "PositionWS";
                spotLightNames[i][1] = prefix + "DirectionWS";
                spotLightNames[i][2] = prefix + "Intensity";
                spotLightNames[i][3] = prefix + "Attenuation";
                spotLightNames[i][4] = prefix + "CutOffCos";
            }

            namesInited = true;
        }

        /// ------ ///

        lightingDataBlock->SetUniform(ambientLightName, &_ambient, sizeof(Vector3));

        int  pointLightsCount    = 0;
        int  spotLightsCount     = 0;
        bool hasDirectionalLight = false;

        for (const auto &light: _lights)
        {
            if (light == nullptr)
                continue;

            if (light->Type == LightType::DIRECTIONAL && !hasDirectionalLight)
            {
                hasDirectionalLight = true;
                auto dir            = light->Rotation * Vector3(0, 0, 1);
                lightingDataBlock->SetUniform(dirLightDirectionName, &dir, sizeof(Vector3));
                lightingDataBlock->SetUniform(dirLightIntensityName, &light->Intensity, sizeof(Vector3));
            }
            else if (light->Type == LightType::POINT && pointLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                lightingDataBlock->SetUniform(pointLightNames[pointLightsCount][0], &light->Position, sizeof(Vector3));
                lightingDataBlock->SetUniform(pointLightNames[pointLightsCount][1], &light->Intensity, sizeof(Vector3));
                lightingDataBlock->SetUniform(pointLightNames[pointLightsCount][2], &light->Attenuation, sizeof(float));
                ++pointLightsCount;
            }
            else if (light->Type == LightType::SPOT && spotLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                auto dir       = light->Rotation * Vector3(0, 0, 1);
                auto cutOffCos = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
                lightingDataBlock->SetUniform(spotLightNames[spotLightsCount][0], &light->Position, sizeof(Vector3));
                lightingDataBlock->SetUniform(spotLightNames[spotLightsCount][1], &dir, sizeof(Vector3));
                lightingDataBlock->SetUniform(spotLightNames[spotLightsCount][2], &light->Intensity, sizeof(Vector3));
                lightingDataBlock->SetUniform(spotLightNames[spotLightsCount][3], &light->Attenuation, sizeof(float));
                lightingDataBlock->SetUniform(spotLightNames[spotLightsCount][4], &cutOffCos, sizeof(float));
                ++spotLightsCount;
            }
        }

        float hasDirLightFloat = hasDirectionalLight ? 1 : -1;
        lightingDataBlock->SetUniform(pointLightsCountName, &pointLightsCount, sizeof(int));
        lightingDataBlock->SetUniform(spotLightsCountName, &spotLightsCount, sizeof(int));
        lightingDataBlock->SetUniform(hasDirLightName, &hasDirLightFloat, sizeof(float));

        lightingDataBlock->UploadData();
    }

    std::vector<DrawCallInfo> DoCulling(const std::vector<Renderer *> &_renderers)
    {
        // TODO: implement culling
        std::vector<DrawCallInfo> info(_renderers.size());
        for (int i = 0; i < _renderers.size(); ++i)
        {
            info[i] = {_renderers[i]->GetGeometry(),
                       _renderers[i]->GetMaterial(),
                       {_renderers[i]->GetModelMatrix()},
                       _renderers[i]->GetAABB()};
        }
        return info;
    }

    void Render()
    {
        auto debugGroup = Debug::DebugGroup("Render Frame");

        CHECK_GL(glClearColor(0, 0, 0, 0))
        CHECK_GL(glClearDepth(1))

        Context ctx;
        ctx.DrawCallInfos = DoCulling(ctx.Renderers);

        SetLightingData(ctx.AmbientLight, ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        CHECK_GL(glViewport(0, 0, screenWidth, screenHeight))
        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        if (opaqueRenderPass)
            opaqueRenderPass->Execute(ctx);
        if (skyboxPass)
            skyboxPass->Execute(ctx);
        if (transparentRenderPass)
            transparentRenderPass->Execute(ctx);

#if OPENGL_STUDY_EDITOR
        if (fallbackRenderPass)
            fallbackRenderPass->Execute(ctx);
        if (gizmosPass)
            gizmosPass->Execute(ctx);

        Gizmos::ClearDrawInfos();
#endif

        Debug::CheckOpenGLError();
    }

    std::vector<DrawCallInfo> BatchDrawCalls(const std::vector<DrawCallInfo> &_drawCalls, std::vector<std::vector<Matrix4x4>> &_instancedMatricesMap)
    {
        std::vector<DrawCallInfo> batchedDrawCalls;
        batchedDrawCalls.reserve(_drawCalls.size());

        std::unordered_map<std::pair<Material *, DrawableGeometry *>, int, DrawCallInfoHash> instancingMap;
        for (const auto &info: _drawCalls)
        {
            if (!info.Material->GetShader()->SupportInstancing())
            {
                batchedDrawCalls.push_back(info);
                continue;
            }

            auto index = batchedDrawCalls.size();
            auto pair  = std::make_pair<Material *, DrawableGeometry *>(info.Material.get(), info.Geometry.get());
            if (!instancingMap.contains(pair))
            {
                batchedDrawCalls.push_back(info);
                batchedDrawCalls[index].InstancedIndex = _instancedMatricesMap.size();
                _instancedMatricesMap.push_back({{info.ModelMatrix}});
                instancingMap[pair] = index;
                continue;
            }

            auto &drawCall = batchedDrawCalls[instancingMap[pair]];
            drawCall.AABB  = drawCall.AABB.Combine(info.AABB);

            auto &matrices = _instancedMatricesMap[drawCall.InstancedIndex];
            matrices.push_back(info.ModelMatrix);

            if (matrices.size() >= MAX_INSTANCING_COUNT)
                instancingMap.erase(pair);
        }

        return batchedDrawCalls;
    }

    void FillMatrices(const DrawCallInfo &_info, const std::vector<std::vector<Matrix4x4>> &_instancedMatricesMap)
    {
        static const std::string modelMatrixName       = "_ModelMatrix";
        static const std::string modelNormalMatrixName = "_ModelNormalMatrix";

        if (_info.Instanced())
        {
            auto &matrices = _instancedMatricesMap[_info.InstancedIndex];
            auto  count    = matrices.size();

            std::vector<Matrix4x4> modelNormalMatrices(count);
            for (int i = 0; i < count; ++i)
                modelNormalMatrices[i] = matrices[i].Invert().Transpose();

            auto matricesSize        = sizeof(Matrix4x4) * count;
            auto normalsMatrixOffset = sizeof(Matrix4x4) * MAX_INSTANCING_COUNT;
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer))
            CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, matricesSize, matrices.data()))
            CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, normalsMatrixOffset, matricesSize, modelNormalMatrices.data()))
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0))
        }
        else
        {
            Shader::SetGlobalMatrix(modelMatrixName, _info.ModelMatrix);
            Shader::SetGlobalMatrix(modelNormalMatrixName, _info.ModelMatrix.Invert().Transpose());
        }
    }

    void SetInstancingEnabled(bool _enabled)
    {
        // make sure VAO is bound before calling this function
        if (_enabled)
        {
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer))

            // matrix4x4 requires 4 vertex attributes
            for (int i = 0; i < 8; ++i)
            {
                CHECK_GL(glEnableVertexAttribArray(INSTANCING_BASE_VERTEX_ATTRIB + i))
                CHECK_GL(glVertexAttribDivisor(INSTANCING_BASE_VERTEX_ATTRIB + i, 1))
            }

            auto vec4Size = sizeof(Vector4);

            // model matrix
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(0)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(1 * vec4Size)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(2 * vec4Size)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(3 * vec4Size)))

            // normal matrix
            auto offset = sizeof(Matrix4x4) * MAX_INSTANCING_COUNT;
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 1 * vec4Size)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 2 * vec4Size)))
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 3 * vec4Size)))

            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0))
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                CHECK_GL(glDisableVertexAttribArray(INSTANCING_BASE_VERTEX_ATTRIB + i))
                CHECK_GL(glVertexAttribDivisor(INSTANCING_BASE_VERTEX_ATTRIB + i, 0))
            }
        }
    }

    void Draw(const std::vector<DrawCallInfo> &_drawCallInfos, const RenderSettings &_settings)
    {
        // filter draw calls
        std::vector<DrawCallInfo> drawCalls;
        drawCalls.reserve(_drawCallInfos.size());
        copy_if(_drawCallInfos.begin(), _drawCallInfos.end(), std::back_inserter(drawCalls), _settings.Filter);

        std::vector<std::vector<Matrix4x4>> instancedMatricesMap;
        drawCalls = BatchDrawCalls(drawCalls, instancedMatricesMap);

        // sort draw calls
        if (_settings.Sorting != DrawCallInfo::Sorting::NO_SORTING)
            std::sort(drawCalls.begin(), drawCalls.end(), DrawCallInfo::Comparer {_settings.Sorting, lastCameraPosition});

        for (const auto &info: drawCalls)
        {
            CHECK_GL(glBindVertexArray(info.Geometry->GetVertexArrayObject()))

            FillMatrices(info, instancedMatricesMap);

            if (info.Instanced())
                SetInstancingEnabled(true);

            auto        type       = info.Geometry->GetGeometryType();
            auto        count      = info.Geometry->GetElementsCount();
            auto        hasIndexes = info.Geometry->HasIndexes();
            const auto &shader     = info.Material->GetShader();

            for (int i = 0; i < shader->PassesCount(); ++i)
            {
                if (!_settings.TagsMatch(*shader, i))
                    continue;

                shader->Use(i);

                Shader::SetPropertyBlock(info.Material->GetPropertyBlock());

                if (info.Instanced())
                {
                    auto instanceCount = instancedMatricesMap[info.InstancedIndex].size();
                    if (hasIndexes)
                    {
                        CHECK_GL(glDrawElementsInstanced(type, count, GL_UNSIGNED_INT, nullptr, instanceCount))
                    }
                    else
                    {
                        CHECK_GL(glDrawArraysInstanced(type, 0, count, instanceCount))
                    }
                }
                else
                {
                    if (hasIndexes)
                    {
                        CHECK_GL(glDrawElements(type, count, GL_UNSIGNED_INT, nullptr))
                    }
                    else
                    {
                        CHECK_GL(glDrawArrays(type, 0, count))
                    }
                }
            }

            if (info.Instanced())
                SetInstancingEnabled(false);

            CHECK_GL(glBindVertexArray(0))
        }
    }

    void Reshape(int _width, int _height)
    {
        screenWidth  = _width;
        screenHeight = _height;
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
        static const std::string vpMatrixName   = "_VPMatrix";
        static const std::string cameraPosName  = "_CameraPosWS";
        static const std::string nearClipName   = "_NearClipPlane";
        static const std::string cameraFwdName  = "_CameraFwdWS";
        static const std::string farClipName    = "_FarClipPlane";

        auto cameraPosWS   = _viewMatrix.Invert().GetPosition();
        auto nearClipPlane = Camera::Current->GetNearClipPlane();
        auto farClipPlane  = Camera::Current->GetFarClipPlane();
        auto vpMatrix = _projectionMatrix * _viewMatrix;
        auto cameraFwdWS = Camera::Current->GetRotation() * Vector3{0, 0, 1};

        cameraDataBlock->SetUniform(vpMatrixName, &vpMatrix, sizeof(Matrix4x4));
        cameraDataBlock->SetUniform(cameraPosName, &cameraPosWS, sizeof(Vector3));
        cameraDataBlock->SetUniform(nearClipName, &nearClipPlane, sizeof(float));
        cameraDataBlock->SetUniform(farClipName, &farClipPlane, sizeof(float));
        cameraDataBlock->SetUniform(cameraFwdName, &cameraFwdWS, sizeof(Vector3));
        cameraDataBlock->UploadData();

        lastCameraPosition = cameraPosWS;
    }

    const std::string &GetGlobalShaderDirectives()
    {
        static constexpr int GLSL_VERSION = OPENGL_MAJOR_VERSION * 100 + OPENGL_MINOR_VERSION * 10;

        // clang-format off
        static std::string globalShaderDirectives = "#version " + std::to_string(GLSL_VERSION) + "\n"
                                                    "#define MAX_POINT_LIGHT_SOURCES " + std::to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                                    "#define MAX_SPOT_LIGHT_SOURCES " + std::to_string(MAX_SPOT_LIGHT_SOURCES) + "\n"
                                                    "#define MAX_INSTANCING_COUNT " + std::to_string(MAX_INSTANCING_COUNT) + "\n";
        // clang-format on

        return globalShaderDirectives;
    }

    void SetRenderTargets(const std::shared_ptr<Texture> &_colorAttachment, int colorLevel, int colorLayer,
                          const std::shared_ptr<Texture> &_depthAttachment, int depthLevel, int depthLayer)
    {
        if (!_colorAttachment && !_depthAttachment)
        {
            CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0))
            return;
        }

        CHECK_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer))

        if (_colorAttachment)
            _colorAttachment->Attach(GL_COLOR_ATTACHMENT0, colorLevel, colorLayer);
        else
        {
            CHECK_GL(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0))
        }

        if (_depthAttachment)
            _depthAttachment->Attach(GL_DEPTH_ATTACHMENT, depthLevel, depthLayer);
        else
        {
            CHECK_GL(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0))
        }
    }
} // namespace Graphics