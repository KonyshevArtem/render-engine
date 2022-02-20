#include "graphics.h"
#include "camera/camera.h"
#include "context.h"
#include "core_debug/debug.h"
#include "gizmos/gizmos.h"
#include "gizmos/gizmos_pass.h"
#include "light/light.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "passes/skybox_pass.h"
#include "shader/shader.h"
#include "uniform_block.h"
#include "vector4/vector4.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

namespace Graphics
{
    constexpr int MAX_POINT_LIGHT_SOURCES = 3;
    constexpr int MAX_SPOT_LIGHT_SOURCES  = 3;

    std::unique_ptr<UniformBlock> lightingDataBlock;
    std::unique_ptr<UniformBlock> cameraDataBlock;
    std::shared_ptr<UniformBlock> shadowsDataBlock;

    std::unique_ptr<ShadowCasterPass> shadowCasterPass;
    std::unique_ptr<RenderPass>       opaqueRenderPass;
    std::unique_ptr<RenderPass>       tranparentRenderPass;
    std::unique_ptr<SkyboxPass>       skyboxPass;
    std::unique_ptr<GizmosPass>       gizmosPass;

    int screenWidth  = 0;
    int screenHeight = 0;

    void InitCulling()
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }

    void InitDepth()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRange(0, 1);
    }

    void InitFramebuffer()
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
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
        opaqueRenderPass     = std::make_unique<RenderPass>("Opaque", Renderer::Sorting::FRONT_TO_BACK, Renderer::Filter::Opaque(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        tranparentRenderPass = std::make_unique<RenderPass>("Transparent", Renderer::Sorting::BACK_TO_FRONT, Renderer::Filter::Transparent(), 0);
        shadowCasterPass     = std::make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, shadowsDataBlock);
        skyboxPass           = std::make_unique<SkyboxPass>();
        gizmosPass           = std::make_unique<GizmosPass>();
    }

    void Init()
    {
#ifdef OPENGL_STUDY_WINDOWS
        auto result = glewInit();
        if (result != GLEW_OK)
            throw;
#endif

        Gizmos::Init();

        InitCulling();
        InitDepth();
        InitFramebuffer();
        InitUniformBlocks();
        InitPasses();
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

    void Render()
    {
        Debug::PushDebugGroup("Render Frame");

        glClearColor(0, 0, 0, 0);
        glClearDepth(1);

        Context ctx;

        SetLightingData(ctx.AmbientLight, ctx.Lights);

        if (shadowCasterPass)
            shadowCasterPass->Execute(ctx);

        glViewport(0, 0, screenWidth, screenHeight);
        SetCameraData(ctx.ViewMatrix, ctx.ProjectionMatrix);

        if (opaqueRenderPass)
            opaqueRenderPass->Execute(ctx);
        if (skyboxPass)
            skyboxPass->Execute(ctx);
        if (tranparentRenderPass)
            tranparentRenderPass->Execute(ctx);
        if (gizmosPass)
            gizmosPass->Execute(ctx);

        Debug::CheckOpenGLError();
        Debug::PopDebugGroup();

        Gizmos::ClearDrawInfos();
    }

    void Reshape(int _width, int _height)
    {
        screenWidth  = _width;
        screenHeight = _height;
        glViewport(0, 0, _width, _height);
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
    }

    const std::string &GetGlobalShaderDirectives()
    {
        static constexpr int GLSL_VERSION = 410;

        static std::string globalShaderDirectives;

        if (globalShaderDirectives.empty())
        {
            // clang-format off
            globalShaderDirectives = "#version " + std::to_string(GLSL_VERSION) + "\n"
                                    "#define MAX_POINT_LIGHT_SOURCES " + std::to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                    "#define MAX_SPOT_LIGHT_SOURCES " + std::to_string(MAX_SPOT_LIGHT_SOURCES) + "\n";
            // clang-format on
        }

        return globalShaderDirectives;
    }
} // namespace Graphics