#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "graphics.h"
#include "../camera/camera.h"
#include "../light/light_data.h"
#include "../scene/scene.h"
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"

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
    glGenBuffers(1, &LightingUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, LightingUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, LIGHTING_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &CameraDataUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, CAMERA_DATA_UNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, CameraDataUniformBuffer, 0, CAMERA_DATA_UNIFORM_SIZE);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, LightingUniformBuffer, 0, LIGHTING_UNIFORM_SIZE);
}

void Graphics::Render()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateLightingData();
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

void Graphics::UpdateCameraData(Vector3 _cameraPosWs, Matrix4x4 _viewMatrix, Matrix4x4 _projectionMatrix)
{
    long matrixSize = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, matrixSize, matrixSize, &_viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * matrixSize, sizeof(Vector4), &_cameraPosWs);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, matrixSize, &_projectionMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Graphics::UpdateLightingData()
{
    LightData lights[MAX_LIGHT_SOURCES];

    LightData dirLight;
    dirLight.PosOrDirWS    = Vector3 {0, -0.3f, 1};
    dirLight.Intensity     = Vector4(1, 1, 1, 1);
    dirLight.IsDirectional = true;

    LightData pointLight;
    pointLight.PosOrDirWS    = Vector3(-3, -3, -4);
    pointLight.Intensity     = Vector4(1, 0, 0, 1);
    pointLight.IsDirectional = false;
    pointLight.Attenuation   = 0.3f;

    lights[0] = dirLight;
    lights[1] = pointLight;

    Vector4 ambientLight = Vector4(0.05f, 0.05f, 0.05f, 1);

    int  lightsCount   = 2;
    long lightDataSize = sizeof(LightData) * MAX_LIGHT_SOURCES;
    glBindBuffer(GL_UNIFORM_BUFFER, LightingUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, lightDataSize, &lights);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize, sizeof(Vector4), &ambientLight);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize + sizeof(Vector4), sizeof(int), &lightsCount); // NOLINT(cppcoreguidelines-narrowing-conversions)
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Graphics::~Graphics()
{
    glDeleteBuffers(1, &CameraDataUniformBuffer);
    glDeleteBuffers(1, &LightingUniformBuffer);
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

const vector<string> &Graphics::GetShaderCompilationDefines()
{
    if (ShaderCompilationDefines.empty())
    {
        ShaderCompilationDefines.push_back("#version " + to_string(GLSL_VERSION) + "\n");
        ShaderCompilationDefines.push_back("#define MAX_LIGHT_SOURCES " + to_string(MAX_LIGHT_SOURCES) + "\n");
    }

    return ShaderCompilationDefines;
}
