#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "graphics.h"
#include "../camera/camera.h"
#include "GLUT/glut.h"
#include "context.h"
#include "uniform_block.h"
#include <memory>

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
    glFrontFace(GL_CW);
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
    auto fullShader   = Shader::Load("shaders/standard.glsl", vector<string> {"_RECEIVE_SHADOWS"});
    CameraDataBlock   = make_unique<UniformBlock>(fullShader, "CameraData", 0);
    LightingDataBlock = make_unique<UniformBlock>(fullShader, "Lighting", 1);
    ShadowsDataBlock  = make_shared<UniformBlock>(fullShader, "Shadows", 2);
}

void Graphics::InitPasses()
{
    m_RenderPass       = make_unique<RenderPass>();
    m_ShadowCasterPass = make_unique<ShadowCasterPass>(MAX_SPOT_LIGHT_SOURCES, ShadowsDataBlock);
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

    glutSwapBuffers();
    glutPostRedisplay();
}

void Graphics::Reshape(int _width, int _height)
{
    ScreenWidth  = _width;
    ScreenHeight = _height;
    glViewport(0, 0, _width, _height);
}

void Graphics::SetCameraData(Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix)
{
    long      matrixSize  = sizeof(Matrix4x4);
    Matrix4x4 cameraWS    = _viewMatrix.Invert();
    Vector3   cameraPosWS = Vector3(cameraWS.m30, cameraWS.m31, cameraWS.m32);

    CameraDataBlock->SetUniform("_ProjMatrix", &_projectionMatrix, matrixSize);
    CameraDataBlock->SetUniform("_ViewMatrix", &_viewMatrix, matrixSize);
    CameraDataBlock->SetUniform("_CameraPosWS", &cameraPosWS, sizeof(Vector4));
    CameraDataBlock->SetUniform("_NearClipPlane", &Camera::Current->NearClipPlane, sizeof(float));
    CameraDataBlock->SetUniform("_FarClipPlane", &Camera::Current->FarClipPlane, sizeof(float));
}

void Graphics::SetLightingData(Vector4 _ambient, const vector<shared_ptr<Light>> &_lights)
{
    LightingDataBlock->SetUniform("_AmbientLight", &_ambient, sizeof(Vector4));

    int  pointLightsCount    = 0;
    int  spotLightsCount     = 0;
    bool hasDirectionalLight = false;

    for (const auto &light: _lights)
    {
        if (light->Type == DIRECTIONAL && !hasDirectionalLight)
        {
            hasDirectionalLight = true;
            Vector3 dir         = light->Rotation * Vector3(0, 0, -1);
            LightingDataBlock->SetUniform("_DirectionalLight.DirectionWS", &dir, sizeof(Vector3));
            LightingDataBlock->SetUniform("_DirectionalLight.Intensity", &light->Intensity, sizeof(Vector4));
        }
        else if (light->Type == POINT && pointLightsCount < MAX_POINT_LIGHT_SOURCES)
        {
            string prefix = "_PointLights[" + to_string(pointLightsCount) + "].";
            LightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
            LightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector4));
            LightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
            ++pointLightsCount;
        }
        else if (light->Type == SPOT && spotLightsCount < MAX_POINT_LIGHT_SOURCES)
        {
            Vector3 dir       = light->Rotation * Vector3(0, 0, -1);
            float   cutOffCos = cosf(light->CutOffAngle * (float) M_PI / 180);
            string  prefix    = "_SpotLights[" + to_string(spotLightsCount) + "].";
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

const string &Graphics::GetShaderCompilationDefines()
{
    if (ShaderCompilationDefine.empty())
    {
        // clang-format off
        ShaderCompilationDefine = "#version " + to_string(GLSL_VERSION) + "\n"
                                  "#define MAX_POINT_LIGHT_SOURCES " + to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                  "#define MAX_SPOT_LIGHT_SOURCES " + to_string(MAX_SPOT_LIGHT_SOURCES) + "\n";
        // clang-format on
    }

    return ShaderCompilationDefine;
}
