#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "skybox_pass.h"
#include "../../../math/matrix4x4/matrix4x4.h"
#include "../../../math/quaternion/quaternion.h"
#include "../../../math/vector3/vector3.h"
#include "../../cubemap/cubemap.h"
#include "../../mesh/cube/cube_mesh.h"
#include "../../shader/shader.h"
#include "../context.h"
#include "../graphics.h"

SkyboxPass::SkyboxPass()
{
    m_Cubemap = Cubemap::Load("resources/textures/skybox/x_positive.png",
                              "resources/textures/skybox/x_negative.png",
                              "resources/textures/skybox/y_positive.png",
                              "resources/textures/skybox/y_negative.png",
                              "resources/textures/skybox/z_positive.png",
                              "resources/textures/skybox/z_negative.png");

    m_Shader = Shader::Load("resources/shaders/skybox.glsl", vector<string>());

    m_Mesh = make_shared<CubeMesh>();
    m_Mesh->Init();
}

void SkyboxPass::Execute(const shared_ptr<Context> &_ctx)
{
    if (m_Mesh == nullptr || m_Shader == nullptr || m_Cubemap == nullptr)
        return;

    glCullFace(GL_FRONT);
    glViewport(0, 0, Graphics::ScreenWidth, Graphics::ScreenHeight);

    Graphics::SetCameraData(_ctx->ViewMatrix, _ctx->ProjectionMatrix);

    glUseProgram(m_Shader->m_Program);
    glBindVertexArray(m_Mesh->m_VertexArrayObject);

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx->ViewMatrix.Invert().GetPosition());
    m_Shader->SetUniform("_ModelMatrix", &modelMatrix);

    int unit = 0;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Cubemap->m_Texture);
    glBindSampler(unit, m_Cubemap->m_Sampler);
    m_Shader->SetUniform("_Skybox", &unit);

    glDrawElements(GL_TRIANGLES, m_Mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glCullFace(GL_BACK);
}
