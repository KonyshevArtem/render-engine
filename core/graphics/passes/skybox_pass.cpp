#include "skybox_pass.h"
#include "../../../math/matrix4x4/matrix4x4.h"
#include "../../../math/vector3/vector3.h"
#include "../../cubemap/cubemap.h"
#include "../../fbx_asset/fbx_asset.h"
#include "../../mesh/mesh.h"
#include "../../shader/shader.h"
#include "../context.h"
#include "../graphics.h"

void SkyboxPass::Execute(const Context &_ctx)
{
    static shared_ptr<Mesh>   mesh   = nullptr;
    static shared_ptr<Shader> shader = nullptr;

    if (mesh == nullptr)
        mesh = FBXAsset::Load("resources/models/cube.fbx")->GetMesh(0);
    if (shader == nullptr)
        shader = Shader::Load("resources/shaders/skybox/skybox.shader", vector<string>());

    if (mesh == nullptr || shader == nullptr || _ctx.Skybox == nullptr)
        return;

    if (!shader->Use())
        return;

    glCullFace(GL_FRONT);
    glViewport(0, 0, Graphics::GetScreenWidth(), Graphics::GetScreenHeight());

    Graphics::SetCameraData(_ctx.ViewMatrix, _ctx.ProjectionMatrix);

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx.ViewMatrix.Invert().GetPosition());
    Shader::SetUniform("_ModelMatrix", &modelMatrix);
    Shader::SetTextureUniform("_Skybox", *_ctx.Skybox);

    mesh->Draw();

    Shader::DetachCurrentShader();
    glCullFace(GL_BACK);
}
