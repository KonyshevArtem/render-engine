#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "graphics.h"
#include "../camera/camera.h"
#include "../light/light.h"
#include "../shader/shader.h"
#include "context.h"
#include "passes/render_pass.h"
#include "passes/shadow_caster_pass.h"
#include "passes/skybox_pass.h"
#include "uniform_block.h"
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#include <memory>

unique_ptr<UniformBlock>     Graphics::LightingDataBlock;
unique_ptr<UniformBlock>     Graphics::CameraDataBlock;
shared_ptr<UniformBlock>     Graphics::ShadowsDataBlock;
unique_ptr<ShadowCasterPass> Graphics::m_ShadowCasterPass;
unique_ptr<RenderPass>       Graphics::m_RenderPass;
unique_ptr<SkyboxPass>       Graphics::m_SkyboxPass;

string Graphics::m_GlobalShaderDirectives;

int Graphics::m_ScreenWidth  = 0;
int Graphics::m_ScreenHeight = 0;

void Graphics::Init(int _argc, char **_argv)
{
    glutInit(&_argc, _argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_3_2_CORE_PROFILE | GLUT_DEPTH);
    glutInitWindowSize(1024, 720);
    glutCreateWindow("OpenGL");

    InitCulling();
    InitDepth();
    InitFramebuffer();
    InitUniformBlocks();
    InitPasses();
}

void Graphics::InitCulling()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Graphics::InitDepth()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0, 1);
}

void Graphics::InitFramebuffer()
{
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void Graphics::InitUniformBlocks()
{
    auto fullShader   = Shader::Load("resources/shaders/standard/standard.shader", vector<string> {"_RECEIVE_SHADOWS"});
    CameraDataBlock   = make_unique<UniformBlock>(fullShader, "CameraData", 0);
    LightingDataBlock = make_unique<UniformBlock>(fullShader, "Lighting", 1);
    ShadowsDataBlock  = make_shared<UniformBlock>(fullShader, "Shadows", 2);
}

void Graphics::InitPasses()
{
    m_RenderPass       = make_unique<RenderPass>();
    m_ShadowCasterPass = make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, ShadowsDataBlock);
    m_SkyboxPass       = make_unique<SkyboxPass>();
}

void Graphics::Render()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);

    auto ctx = make_shared<Context>();

    SetLightingData(ctx->AmbientLight, ctx->Lights);

    if (m_ShadowCasterPass != nullptr)
        m_ShadowCasterPass->Execute(ctx);
    if (m_RenderPass != nullptr)
        m_RenderPass->Execute(ctx);
    if (m_SkyboxPass != nullptr)
        m_SkyboxPass->Execute(ctx);

    glutSwapBuffers();
    glutPostRedisplay();

    GLenum error = glGetError();
    if (error != 0)
        printf("%s\n", gluErrorString(error));
}

void Graphics::Reshape(int _width, int _height)
{
    m_ScreenWidth  = _width;
    m_ScreenHeight = _height;
    glViewport(0, 0, _width, _height);
}

void Graphics::SetCameraData(const Matrix4x4 &_viewMatrix, const Matrix4x4 &_projectionMatrix)
{
    auto matrixSize    = sizeof(Matrix4x4);
    auto cameraPosWS   = _viewMatrix.Invert().GetPosition();
    auto nearClipPlane = Camera::Current->GetNearClipPlane();
    auto farClipPlane  = Camera::Current->GetFarClipPlane();

    CameraDataBlock->SetUniform("_ProjMatrix", &_projectionMatrix, matrixSize);
    CameraDataBlock->SetUniform("_ViewMatrix", &_viewMatrix, matrixSize);
    CameraDataBlock->SetUniform("_CameraPosWS", &cameraPosWS, sizeof(Vector4));
    CameraDataBlock->SetUniform("_NearClipPlane", &nearClipPlane, sizeof(float));
    CameraDataBlock->SetUniform("_FarClipPlane", &farClipPlane, sizeof(float));
}

void Graphics::SetLightingData(const Vector4 &_ambient, const vector<shared_ptr<Light>> &_lights)
{
    LightingDataBlock->SetUniform("_AmbientLight", &_ambient, sizeof(Vector4));

    int  pointLightsCount    = 0;
    int  spotLightsCount     = 0;
    bool hasDirectionalLight = false;

    for (const auto &light: _lights)
    {
        if (light->Type == LightType::DIRECTIONAL && !hasDirectionalLight)
        {
            hasDirectionalLight = true;
            auto dir            = light->Rotation * Vector3(0, 0, -1);
            LightingDataBlock->SetUniform("_DirectionalLight.DirectionWS", &dir, sizeof(Vector3));
            LightingDataBlock->SetUniform("_DirectionalLight.Intensity", &light->Intensity, sizeof(Vector4));
        }
        else if (light->Type == LightType::POINT && pointLightsCount < MAX_POINT_LIGHT_SOURCES)
        {
            auto prefix = "_PointLights[" + to_string(pointLightsCount) + "].";
            LightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
            LightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector4));
            LightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
            ++pointLightsCount;
        }
        else if (light->Type == LightType::SPOT && spotLightsCount < MAX_POINT_LIGHT_SOURCES)
        {
            auto dir       = light->Rotation * Vector3(0, 0, -1);
            auto cutOffCos = cosf(light->CutOffAngle * static_cast<float>(M_PI) / 180);
            auto prefix    = "_SpotLights[" + to_string(spotLightsCount) + "].";
            LightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
            LightingDataBlock->SetUniform(prefix + "DirectionWS", &dir, sizeof(Vector3));
            LightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector4));
            LightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
            LightingDataBlock->SetUniform(prefix + "CutOffCos", &cutOffCos, sizeof(float));
            ++spotLightsCount;
        }
    }

    LightingDataBlock->SetUniform("_PointLightsCount", &pointLightsCount, sizeof(int));
    LightingDataBlock->SetUniform("_SpotLightsCount", &spotLightsCount, sizeof(int));
    LightingDataBlock->SetUniform("_HasDirectionalLight", &hasDirectionalLight, sizeof(bool));
}

const string &Graphics::GetGlobalShaderDirectives()
{
    if (m_GlobalShaderDirectives.empty())
    {
        // clang-format off
        m_GlobalShaderDirectives = "#version " + to_string(GLSL_VERSION) + "\n"
                                  "#define MAX_POINT_LIGHT_SOURCES " + to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                  "#define MAX_SPOT_LIGHT_SOURCES " + to_string(MAX_SPOT_LIGHT_SOURCES) + "\n";
        // clang-format on
    }

    return m_GlobalShaderDirectives;
}
