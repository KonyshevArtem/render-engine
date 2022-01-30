#include "graphics.h"
#include "../../math/vector4/vector4.h"
#include "../camera/camera.h"
#include "../light/light.h"
#include "../shader/shader.h"
#include "context.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "passes/skybox_pass.h"
#include "uniform_block.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/freeglut.h>
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#endif
#include <memory>

namespace Graphics
{
    constexpr int MAX_POINT_LIGHT_SOURCES = 3;
    constexpr int MAX_SPOT_LIGHT_SOURCES  = 3;

    unique_ptr<UniformBlock> lightingDataBlock;
    unique_ptr<UniformBlock> cameraDataBlock;
    shared_ptr<UniformBlock> shadowsDataBlock;

    unique_ptr<ShadowCasterPass> shadowCasterPass;
    unique_ptr<RenderPass>       opaqueRenderPass;
    unique_ptr<RenderPass>       tranparentRenderPass;
    unique_ptr<SkyboxPass>       skyboxPass;

    int screenWidth  = 0;
    int screenHeight = 0;

    void InitCulling()
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
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
        auto fullShader   = Shader::Load("resources/shaders/standard/standard.shader", vector<string> {"_RECEIVE_SHADOWS"});
        cameraDataBlock   = make_unique<UniformBlock>(*fullShader, "CameraData", 0);
        lightingDataBlock = make_unique<UniformBlock>(*fullShader, "Lighting", 1);
        shadowsDataBlock  = make_shared<UniformBlock>(*fullShader, "Shadows", 2);
    }

    void InitPasses()
    {
        opaqueRenderPass     = make_unique<RenderPass>(Renderer::Sorting::FRONT_TO_BACK, Renderer::Filter::Opaque(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        tranparentRenderPass = make_unique<RenderPass>(Renderer::Sorting::BACK_TO_FRONT, Renderer::Filter::Transparent(), 0);
        shadowCasterPass     = make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, shadowsDataBlock);
        skyboxPass           = make_unique<SkyboxPass>();
    }

    void Init(int _argc, char **_argv)
    {
        unsigned int displayModeFlags = GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH;
#ifdef OPENGL_STUDY_MACOS
        displayModeFlags |= GLUT_3_2_CORE_PROFILE;
#endif

		glutInit(&_argc, _argv);
		glutInitDisplayMode(displayModeFlags);
		glutInitWindowSize(1024, 720);
        glutCreateWindow("OpenGL");

#ifdef OPENGL_STUDY_WINDOWS
        auto result = glewInit();
        if (result != GLEW_OK)
            throw;
#endif

        InitCulling();
        InitDepth();
        InitFramebuffer();
        InitUniformBlocks();
        InitPasses();
    }

    void SetLightingData(const Vector3 &_ambient, const vector<Light *> &_lights)
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
                auto dir            = light->Rotation * Vector3(0, 0, -1);
                lightingDataBlock->SetUniform("_DirectionalLight.DirectionWS", &dir, sizeof(Vector3));
                lightingDataBlock->SetUniform("_DirectionalLight.Intensity", &light->Intensity, sizeof(Vector3));
            }
            else if (light->Type == LightType::POINT && pointLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                auto prefix = "_PointLights[" + to_string(pointLightsCount) + "].";
                lightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector3));
                lightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
                ++pointLightsCount;
            }
            else if (light->Type == LightType::SPOT && spotLightsCount < MAX_POINT_LIGHT_SOURCES)
            {
                auto dir       = light->Rotation * Vector3(0, 0, -1);
                auto cutOffCos = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
                auto prefix    = "_SpotLights[" + to_string(spotLightsCount) + "].";
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
        glClearColor(0, 0, 0, 0);
        glClearDepth(1);

        Context ctx;

        SetLightingData(ctx.AmbientLight, ctx.Lights);

        if (shadowCasterPass != nullptr)
            shadowCasterPass->Execute(ctx);
        if (opaqueRenderPass != nullptr)
            opaqueRenderPass->Execute(ctx);
        if (skyboxPass != nullptr)
            skyboxPass->Execute(ctx);
        if (tranparentRenderPass != nullptr)
            tranparentRenderPass->Execute(ctx);

        glutSwapBuffers();
        glutPostRedisplay();

        GLenum error = glGetError();
        if (error != 0)
            printf("%s\n", gluErrorString(error));
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

    const string &GetGlobalShaderDirectives()
    {
        static constexpr int GLSL_VERSION = 410;

        static string globalShaderDirectives;

        if (globalShaderDirectives.empty())
        {
            // clang-format off
            globalShaderDirectives = "#version " + to_string(GLSL_VERSION) + "\n"
                                    "#define MAX_POINT_LIGHT_SOURCES " + to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                    "#define MAX_SPOT_LIGHT_SOURCES " + to_string(MAX_SPOT_LIGHT_SOURCES) + "\n";
            // clang-format on
        }

        return globalShaderDirectives;
    }
} // namespace Graphics