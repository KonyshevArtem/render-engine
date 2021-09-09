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
    InitShadows();
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

void Graphics::InitShadows()
{
    glGenFramebuffers(1, &ShadowFramebuffer);
    ShadowMap          = Texture::ShadowMap(1024, 1024);
    ShadowCasterShader = Shader::Load("shaders/shadowCaster.glsl", vector<string>());

    glBindFramebuffer(GL_FRAMEBUFFER, ShadowFramebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMap->m_Texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed to init shadow framebuffer, status: 0x%x\n", Status);
        glDeleteFramebuffers(1, &ShadowFramebuffer);
        ShadowMap          = nullptr;
        ShadowCasterShader = nullptr;
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Graphics::Render()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);

    UpdateLightingData(Scene::Current->AmbientLight, Scene::Current->Lights);

    ShadowCasterPass();
    RenderPass();

    glutSwapBuffers();
    glutPostRedisplay();
}

Matrix4x4 m_Test;

void Graphics::ShadowCasterPass()
{
    if (ShadowMap == nullptr || ShadowCasterShader == nullptr)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ShadowFramebuffer);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);

    shared_ptr<Light> light = nullptr;
    for (const auto &l: Scene::Current->Lights)
    {
        if (l->Type == SPOT)
        {
            light = l;
            break;
        }
    }

    if (light == nullptr)
        return;

    Matrix4x4 view = Matrix4x4::Rotation(light->Rotation.Inverse()) * Matrix4x4::Translation(-light->Position);
    Matrix4x4 proj = Matrix4x4::Perspective(light->CutOffAngle * 2, 1, 0.5f, 100);
    m_Test         = proj * view;
    UpdateCameraData(
            light->Position,
            view,
            proj);

    glUseProgram(ShadowCasterShader->m_Program);

    for (const auto &go: Scene::Current->GameObjects)
    {
        glBindVertexArray(go->Mesh->m_VertexArrayObject);

        Matrix4x4 modelMatrix = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        ShadowCasterShader->SetUniform("_ModelMatrix", &modelMatrix);

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void Graphics::RenderPass()
{
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateCameraData(Camera::Current->Position, Camera::Current->GetViewMatrix(), Camera::Current->GetProjectionMatrix());

    for (const auto &go: Scene::Current->GameObjects)
    {
        if (go->Mesh == nullptr || go->Material == nullptr)
            continue;

        auto shader = go->Material->m_Shader != nullptr ? go->Material->m_Shader : Shader::FallbackShader;

        glUseProgram(shader->m_Program);
        glBindVertexArray(go->Mesh->m_VertexArrayObject);

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        shader->SetUniform("_ModelMatrix", &modelMatrix);
        shader->SetUniform("_ModelNormalMatrix", &modelNormalMatrix);

        unordered_map<string, int> textureUnits;
        BindDefaultTextures(shader, textureUnits);
        TransferUniformsFromMaterial(go->Material);

        if (ShadowMap != nullptr && textureUnits.contains("_ShadowMap"))
        {
            int unit = textureUnits["_ShadowMap"];
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, ShadowMap->m_Texture);
            glBindSampler(unit, ShadowMap->m_Sampler);
            shader->SetUniform("_LightViewProjMatrix", &m_Test);
        }

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        for (const auto &pair: textureUnits)
        {
            glActiveTexture(GL_TEXTURE0 + pair.second);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindSampler(pair.second, 0);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
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
    CameraDataBlock->SetUniform("_NearClipPlane", &Camera::Current->NearClipPlane, sizeof(float));
    CameraDataBlock->SetUniform("_FarClipPlane", &Camera::Current->FarClipPlane, sizeof(float));
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

void Graphics::BindDefaultTextures(const shared_ptr<Shader> &_shader, unordered_map<string, int> &_textureUnits)
{
    shared_ptr<Texture> white = Texture::White();

    int unit = 0;
    for (const auto &pair: _shader->m_Uniforms)
    {
        if (pair.second.Type != SAMPLER_2D)
            continue;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, white->m_Texture);
        glBindSampler(unit, white->m_Sampler);

        Vector4 st = Vector4(0, 0, 1, 1);
        _shader->SetUniform(pair.first, &unit);
        _shader->SetUniform(pair.first + "ST", &st);

        _textureUnits[pair.first] = unit;
        ++unit;
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
        // clang-format off
        ShaderCompilationDefine = "#version " + to_string(GLSL_VERSION) + "\n"
                                  "#define MAX_POINT_LIGHT_SOURCES " + to_string(MAX_POINT_LIGHT_SOURCES) + "\n"
                                  "#define MAX_SPOT_LIGHT_SOURCES " + to_string(MAX_SPOT_LIGHT_SOURCES) + "\n";
        // clang-format on
    }

    return ShaderCompilationDefine;
}

Graphics::~Graphics()
{
    glDeleteFramebuffers(1, &ShadowFramebuffer);
}
