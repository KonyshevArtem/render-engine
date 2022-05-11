#include "graphics.h"
#include "camera/camera.h"
#include "context.h"
#include "core_debug/debug.h"
#include "draw_call_info.h"
#include "editor/gizmos/gizmos.h"
#include "editor/gizmos/gizmos_pass.h"
#include "light/light.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "passes/skybox_pass.h"
#include "render_settings.h"
#include "renderer/renderer.h"
#include "shader/shader.h"
#include "uniform_block.h"
#include "vector4/vector4.h"
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
    constexpr int MAX_SPOT_LIGHT_SOURCES        = 3;
    constexpr int MAX_INSTANCING_COUNT          = 256;
    constexpr int INSTANCING_BASE_VERTEX_ATTRIB = 4;

    std::unique_ptr<UniformBlock> lightingDataBlock;
    std::unique_ptr<UniformBlock> cameraDataBlock;
    std::shared_ptr<UniformBlock> shadowsDataBlock;

    std::unique_ptr<ShadowCasterPass> shadowCasterPass;
    std::unique_ptr<RenderPass>       opaqueRenderPass;
    std::unique_ptr<RenderPass>       tranparentRenderPass;
    std::unique_ptr<SkyboxPass>       skyboxPass;

#if OPENGL_STUDY_EDITOR
    std::unique_ptr<RenderPass> fallbackRenderPass;
    std::unique_ptr<GizmosPass> gizmosPass;
#endif

    int screenWidth  = 0;
    int screenHeight = 0;

    Vector3 lastCameraPosition;

    GLuint instancingMatricesBuffer;

    void InitCulling()
    {
        CHECK_GL(glFrontFace(GL_CW));
    }

    void InitDepth()
    {
        CHECK_GL(glEnable(GL_DEPTH_TEST));
        CHECK_GL(glDepthMask(GL_TRUE));
        CHECK_GL(glDepthRange(0, 1));
    }

    void InitFramebuffer()
    {
        CHECK_GL(glEnable(GL_FRAMEBUFFER_SRGB));
    }

    void InitUniformBlocks()
    {
        auto fullShader   = Shader::Load("resources/shaders/standard/standard.shader", {"_RECEIVE_SHADOWS"});
        cameraDataBlock   = std::make_unique<UniformBlock>(*fullShader, "CameraData", 0);
        lightingDataBlock = std::make_unique<UniformBlock>(*fullShader, "Lighting", 1);
        shadowsDataBlock  = std::make_shared<UniformBlock>(*fullShader, "Shadows", 2);
    }

    void InitPasses()
    {
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", DrawCallInfo::Sorting::FRONT_TO_BACK, DrawCallInfo::Filter::Opaque(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, "Forward");
        tranparentRenderPass = std::make_unique<RenderPass>("Transparent", DrawCallInfo::Sorting::BACK_TO_FRONT, DrawCallInfo::Filter::Transparent(), 0, "Forward");
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();

#if OPENGL_STUDY_EDITOR
        fallbackRenderPass = std::make_unique<RenderPass>("Fallback", DrawCallInfo::Sorting::FRONT_TO_BACK, DrawCallInfo::Filter::All(), 0, "Fallback");
        gizmosPass         = std::make_unique<GizmosPass>();
#endif
    }

    void InitInstancing()
    {
        CHECK_GL(glGenBuffers(1, &instancingMatricesBuffer));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer));
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4x4) * MAX_INSTANCING_COUNT * 2, nullptr, GL_STATIC_DRAW));
        CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
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
    }

    void Shutdown()
    {
        CHECK_GL(glDeleteBuffers(1, &instancingMatricesBuffer));
    }

    void SetLightingData(const Vector3 &_ambient, const std::vector<Light *> &_lights)
    {
        lightingDataBlock->SetUniform("_AmbientLight", &_ambient, sizeof(Vector3));

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
                lightingDataBlock->SetUniform("_DirectionalLight.DirectionWS", &dir, sizeof(Vector3));
                lightingDataBlock->SetUniform("_DirectionalLight.Intensity", &light->Intensity, sizeof(Vector3));
            }
            else if (light->Type == LightType::POINT && pointLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                auto prefix = "_PointLights[" + std::to_string(pointLightsCount) + "].";
                lightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
                ++pointLightsCount;
            }
            else if (light->Type == LightType::SPOT && spotLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                auto dir       = light->Rotation * Vector3(0, 0, 1);
                auto cutOffCos = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
                auto prefix    = "_SpotLights[" + std::to_string(spotLightsCount) + "].";
                lightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "DirectionWS", &dir, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
                lightingDataBlock->SetUniform(prefix + "CutOffCos", &cutOffCos, sizeof(float));
                ++spotLightsCount;
            }
        }

        lightingDataBlock->SetUniform("_PointLightsCount", &pointLightsCount, sizeof(int));
        lightingDataBlock->SetUniform("_SpotLightsCount", &spotLightsCount, sizeof(int));
        lightingDataBlock->SetUniform("_HasDirectionalLight", &hasDirectionalLight, sizeof(bool));
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

        CHECK_GL(glClearColor(0, 0, 0, 0));
        CHECK_GL(glClearDepth(1));

        Context ctx;
        ctx.DrawCallInfos = DoCulling(ctx.Renderers);

        SetLightingData(ctx.AmbientLight, ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        CHECK_GL(glViewport(0, 0, screenWidth, screenHeight));
        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        if (opaqueRenderPass)
            opaqueRenderPass->Execute(ctx);
        if (skyboxPass)
            skyboxPass->Execute(ctx);
        if (tranparentRenderPass)
            tranparentRenderPass->Execute(ctx);

#if OPENGL_STUDY_EDITOR
        if (fallbackRenderPass)
            fallbackRenderPass->Execute(ctx);
        if (gizmosPass)
            gizmosPass->Execute(ctx);

        Gizmos::ClearDrawInfos();
#endif

        Debug::CheckOpenGLError(__FILE__, __LINE__);
    }

    std::vector<DrawCallInfo> BatchDrawCalls(const std::vector<DrawCallInfo> &_drawCalls)
    {
        std::vector<DrawCallInfo> batchedDrawCalls;

        std::vector<DrawCallInfo> instancedInfos;
        for (const auto &info: _drawCalls)
        {
            if (info.Material->GetShader()->SupportInstancing())
                instancedInfos.push_back(info);
            else
                batchedDrawCalls.push_back(info);
        }

        std::unordered_map<std::pair<Material *, DrawableGeometry *>, DrawCallInfo, DrawCallInfoHash> instancingMap;
        for (const auto &info: instancedInfos)
        {
            auto pair = std::make_pair<Material *, DrawableGeometry *>(info.Material.get(), info.Geometry.get());
            if (!instancingMap.contains(pair))
            {
                instancingMap[pair] = info;
                continue;
            }

            auto &drawCall = instancingMap[pair];
            drawCall.AABB  = drawCall.AABB.Combine(info.AABB);
            drawCall.ModelMatrices.push_back(info.GetModelMatrix());

            if (drawCall.ModelMatrices.size() >= MAX_INSTANCING_COUNT)
            {
                drawCall.Instanced = true;
                batchedDrawCalls.push_back(drawCall);
                instancingMap.erase(pair);
            }
        }

        for (auto &pair: instancingMap)
        {
            pair.second.Instanced = true;
            batchedDrawCalls.push_back(pair.second);
        };

        return batchedDrawCalls;
    }

    void FillMatrices(const DrawCallInfo &_info)
    {
        if (_info.Instanced)
        {
            auto count = _info.ModelMatrices.size();

            std::vector<Matrix4x4> modelNormalMatrices(count);
            for (int i = 0; i < count; ++i)
                modelNormalMatrices[i] = _info.ModelMatrices[i].Invert().Transpose();

            auto matricesSize        = sizeof(Matrix4x4) * count;
            auto normalsMatrixOffset = sizeof(Matrix4x4) * MAX_INSTANCING_COUNT;
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer));
            CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, matricesSize, _info.ModelMatrices.data()));
            CHECK_GL(glBufferSubData(GL_ARRAY_BUFFER, normalsMatrixOffset, matricesSize, modelNormalMatrices.data()));
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }
        else
        {
            auto matrix = _info.GetModelMatrix();
            Shader::SetGlobalMatrix("_ModelMatrix", matrix);
            Shader::SetGlobalMatrix("_ModelNormalMatrix", matrix.Invert().Transpose());
        }
    }

    void SetInstancingEnabled(bool _enabled)
    {
        // make sure VAO is bound before calling this function
        if (_enabled)
        {
            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, instancingMatricesBuffer));

            // matrix4x4 requires 4 vertex attributes
            for (int i = 0; i < 8; ++i)
            {
                CHECK_GL(glEnableVertexAttribArray(INSTANCING_BASE_VERTEX_ATTRIB + i));
                CHECK_GL(glVertexAttribDivisor(INSTANCING_BASE_VERTEX_ATTRIB + i, 1));
            }

            auto vec4Size = sizeof(Vector4);

            // model matrix
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(0)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(1 * vec4Size)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(2 * vec4Size)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(3 * vec4Size)));

            // normal matrix
            auto offset = sizeof(Matrix4x4) * MAX_INSTANCING_COUNT;
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 1 * vec4Size)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 2 * vec4Size)));
            CHECK_GL(glVertexAttribPointer(INSTANCING_BASE_VERTEX_ATTRIB + 7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(offset + 3 * vec4Size)));

            CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                CHECK_GL(glDisableVertexAttribArray(INSTANCING_BASE_VERTEX_ATTRIB + i));
                CHECK_GL(glVertexAttribDivisor(INSTANCING_BASE_VERTEX_ATTRIB + i, 0));
            }
        }
    }

    void Draw(const std::vector<DrawCallInfo> &_drawCallInfos, const RenderSettings &_settings)
    {
        // filter draw calls
        std::vector<DrawCallInfo> drawCalls;
        copy_if(_drawCallInfos.begin(), _drawCallInfos.end(), std::back_inserter(drawCalls), _settings.Filter);

        drawCalls = BatchDrawCalls(drawCalls);

        // sort draw calls
        if (_settings.Sorting != DrawCallInfo::Sorting::NO_SORTING)
            std::sort(drawCalls.begin(), drawCalls.end(), DrawCallInfo::Comparer {_settings.Sorting, lastCameraPosition});

        for (const auto &info: drawCalls)
        {
            CHECK_GL(glBindVertexArray(info.Geometry->GetVertexArrayObject()));

            FillMatrices(info);

            if (info.Instanced)
                SetInstancingEnabled(true);

            auto        type          = info.Geometry->GetGeometryType();
            auto        count         = info.Geometry->GetElementsCount();
            auto        hasIndexes    = info.Geometry->HasIndexes();
            auto        instanceCount = info.ModelMatrices.size();
            const auto &shader        = info.Material->GetShader();

            for (int i = 0; i < shader->PassesCount(); ++i)
            {
                if (!_settings.TagsMatch(*shader, i))
                    continue;

                shader->Use(i);

                Shader::SetPropertyBlock(info.Material->GetPropertyBlock());

                if (info.Instanced)
                {
                    if (hasIndexes)
                    {
                        CHECK_GL(glDrawElementsInstanced(type, count, GL_UNSIGNED_INT, nullptr, instanceCount));
                    }
                    else
                    {
                        CHECK_GL(glDrawArraysInstanced(type, 0, count, instanceCount));
                    }
                }
                else
                {
                    if (hasIndexes)
                    {
                        CHECK_GL(glDrawElements(type, count, GL_UNSIGNED_INT, nullptr));
                    }
                    else
                    {
                        CHECK_GL(glDrawArrays(type, 0, count));
                    }
                }
            }

            if (info.Instanced)
                SetInstancingEnabled(false);

            CHECK_GL(glBindVertexArray(0));
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
        auto matrixSize    = sizeof(Matrix4x4);
        auto cameraPosWS   = _viewMatrix.Invert().GetPosition();
        auto nearClipPlane = Camera::Current->GetNearClipPlane();
        auto farClipPlane  = Camera::Current->GetFarClipPlane();

        cameraDataBlock->SetUniform("_ProjMatrix", &_projectionMatrix, matrixSize);
        cameraDataBlock->SetUniform("_ViewMatrix", &_viewMatrix, matrixSize);
        cameraDataBlock->SetUniform("_CameraPosWS", &cameraPosWS, sizeof(Vector4));
        cameraDataBlock->SetUniform("_NearClipPlane", &nearClipPlane, sizeof(float));
        cameraDataBlock->SetUniform("_FarClipPlane", &farClipPlane, sizeof(float));

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
} // namespace Graphics