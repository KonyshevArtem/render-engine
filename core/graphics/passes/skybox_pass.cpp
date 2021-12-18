#pragma clang diagnostic push
#pragma ide diagnostic   ignored "OCUnusedMacroInspection"
#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic pop

#include "skybox_pass.h"
#include "../../../math/matrix4x4/matrix4x4.h"
#include "../../../math/vector3/vector3.h"
#include "../../cubemap/cubemap.h"
#include "../../fbx_asset/fbx_asset.h"
#include "../../mesh/mesh.h"
#include "../../shader/shader.h"
#include "../context.h"
#include "../graphics.h"

SkyboxPass::SkyboxPass() :
    m_Shader(Shader::Load("resources/shaders/skybox/skybox.shader", vector<string>())),
    m_Mesh(FBXAsset::Load("resources/models/cube.fbx")->GetMesh(0))
{
}

void SkyboxPass::Execute(const shared_ptr<Context> &_ctx)
{
    if (m_Mesh == nullptr || m_Shader == nullptr || _ctx->Skybox == nullptr)
        return;

    glCullFace(GL_FRONT);
    glViewport(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight());

    Graphics::SetCameraData(_ctx->ViewMatrix, _ctx->ProjectionMatrix);

    m_Shader->Use();

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx->ViewMatrix.Invert().GetPosition());
    m_Shader->SetUniform("_ModelMatrix", &modelMatrix);

    int unit = 0;
    _ctx->Skybox->Bind(unit);
    m_Shader->SetUniform("_Skybox", &unit);

    m_Mesh->Draw();

    Shader::DetachCurrentShader();
    glCullFace(GL_BACK);
}
