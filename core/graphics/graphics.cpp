#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "graphics.h"
#include "../camera/camera.h"
#include "../light/light.h"
#include "../scene/scene.h"
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
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
    CameraDataBlock   = make_unique<UniformBlock>("shaders/common/camera_data.glsl", "CameraData", 0);
    LightingDataBlock = make_unique<UniformBlock>("shaders/common/lighting.glsl", "Lighting", 1);
}

void Graphics::Render()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateLightingData(Scene::Current->AmbientLight, Scene::Current->Lights);
    UpdateCameraData(Camera::Current->Position, Camera::Current->GetViewMatrix(), Camera::Current->GetProjectionMatrix());

    for (const auto &go: Scene::Current->GameObjects)
    {
        if (go->Mesh == nullptr || go->Material == nullptr)
            continue;

        auto shader = go->Material->m_Shader;

        glUseProgram(shader->m_Program);
        glBindVertexArray(go->Mesh->m_VertexArrayObject);

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        shader->SetUniform("_ModelMatrix", &modelMatrix);
        shader->SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

        int textureUnits = 0;
        BindDefaultTextures(shader, textureUnits);
        TransferUniformsFromMaterial(go->Material);

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        for (int i = 0; i < textureUnits; ++i)
            glBindSampler(i, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void Graphics::Reshape(int _width, int _height)
{
    ScreenWidth  = _width;
    ScreenHeight = _height;
    glViewport(0, 0, _width, _height);
}

void Graphics::UpdateCameraData(Vector3 _cameraPosWS, Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix)
{
    long matrixSize = sizeof(Matrix4x4);

    CameraDataBlock->SetUniform("_ProjMatrix", &_projectionMatrix, matrixSize);
    CameraDataBlock->SetUniform("_ViewMatrix", &_viewMatrix, matrixSize);
    CameraDataBlock->SetUniform("_CameraPosWS", &_cameraPosWS, sizeof(Vector4));
}

void Graphics::UpdateLightingData(Vector4 _ambient, const vector<shared_ptr<Light>> &_lights)
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
        else if (light->Type == POINT && pointLightsCount < MAX_LIGHT_SOURCES)
        {
            string prefix = "_PointLights[" + to_string(pointLightsCount) + "].";
            LightingDataBlock->SetUniform(prefix + "PositionWS", &light->Position, sizeof(Vector3));
            LightingDataBlock->SetUniform(prefix + "Intensity", &light->Intensity, sizeof(Vector4));
            LightingDataBlock->SetUniform(prefix + "Attenuation", &light->Attenuation, sizeof(float));
            ++pointLightsCount;
        }
        else if (light->Type == SPOT && spotLightsCount < MAX_LIGHT_SOURCES)
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

void Graphics::BindDefaultTextures(const shared_ptr<Shader> &_shader, int &_textureUnits)
{
    shared_ptr<Texture> white = Texture::White();

    for (const auto &pair: _shader->m_Uniforms)
    {
        if (pair.second.Type != SAMPLER_2D)
            continue;

        glActiveTexture(GL_TEXTURE0 + _textureUnits);
        glBindTexture(GL_TEXTURE_2D, white->m_Texture);
        glBindSampler(_textureUnits, white->m_Sampler);

        Vector4 st = Vector4(0, 0, 1, 1);
        _shader->SetUniform(pair.first, &_textureUnits);
        _shader->SetUniform(pair.first + "ST", &st);

        ++_textureUnits;
    }
}

void Graphics::TransferUniformsFromMaterial(const shared_ptr<Material> &_material)
{
    int textureUnit = 0;
    for (const auto &pair: _material->m_Textures)
    {
        if (pair.second == nullptr)
            continue;

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, pair.second->m_Texture);
        glBindSampler(textureUnit, pair.second->m_Sampler);

        _material->m_Shader->SetUniform(pair.first, &textureUnit);

        ++textureUnit;
    }

    for (const auto &pair: _material->m_Vectors4)
        _material->m_Shader->SetUniform(pair.first, &pair.second);
    for (const auto &pair: _material->m_Floats)
        _material->m_Shader->SetUniform(pair.first, &pair.second);
}

const string &Graphics::GetShaderCompilationDefines()
{
    if (ShaderCompilationDefine.empty())
    {
        ShaderCompilationDefine = "#version " + to_string(GLSL_VERSION) +
                                  "\n"
                                  "#define MAX_LIGHT_SOURCES " + to_string(MAX_LIGHT_SOURCES) +
                                  "\n";
    }

    return ShaderCompilationDefine;
}
