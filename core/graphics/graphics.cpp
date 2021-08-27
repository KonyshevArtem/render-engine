#define GL_SILENCE_DEPRECATION

#include "graphics.h"
#include "../camera/camera.h"
#include "../light/light_data.h"
#include "../scene/scene.h"
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"

void Graphics::Init(int argc, char **argv)
{
    glutInit(&argc, argv);
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

        glUseProgram(go->Material->ShaderPtr->Program);
        glBindVertexArray(go->Mesh->GetVertexArrayObject());

        Matrix4x4 modelMatrix       = Matrix4x4::TRS(go->LocalPosition, go->LocalRotation, go->LocalScale);
        Matrix4x4 modelNormalMatrix = modelMatrix.Invert().Transpose();

        glUniformMatrix4fv(go->Material->ShaderPtr->ModelMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelMatrix);
        glUniformMatrix4fv(go->Material->ShaderPtr->ModelNormalMatrixLocation, 1, GL_FALSE, (const GLfloat *) &modelNormalMatrix);
        glUniform1f(go->Material->ShaderPtr->SmoothnessLocation, go->Material->Smoothness);

        if (go->Material->Albedo != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, go->Material->Albedo->Ptr);
            glBindSampler(0, go->Material->Albedo->Sampler);
            glUniform1i(go->Material->ShaderPtr->AlbedoLocation, 0);
            glUniform4fv(go->Material->ShaderPtr->AlbedoSTLocation, 1, (const GLfloat *) &go->Material->AlbedoST);
        }

        glDrawElements(GL_TRIANGLES, go->Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindSampler(0, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void Graphics::Reshape(int width, int height)
{
    ScreenWidth  = width;
    ScreenHeight = height;
    glViewport(0, 0, width, height);
}

void Graphics::UpdateCameraData(Vector3 cameraPosWS, Matrix4x4 viewMatrix, Matrix4x4 projectionMatrix)
{
    long matrixSize = sizeof(Matrix4x4);
    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, matrixSize, matrixSize, &viewMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * matrixSize, sizeof(Vector4), &cameraPosWS);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, CameraDataUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, matrixSize, &projectionMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Graphics::UpdateLightingData()
{
    const int lightsCount = 3;
    LightData lights[lightsCount];

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

    long lightDataSize = sizeof(LightData) * lightsCount;
    glBindBuffer(GL_UNIFORM_BUFFER, LightingUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, lightDataSize, &lights);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize, sizeof(Vector4), &ambientLight);
    glBufferSubData(GL_UNIFORM_BUFFER, lightDataSize + sizeof(Vector4), sizeof(int), &lightsCount);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Graphics::~Graphics()
{
    glDeleteBuffers(1, &CameraDataUniformBuffer);
    glDeleteBuffers(1, &LightingUniformBuffer);
}
