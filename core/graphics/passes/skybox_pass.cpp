#include "skybox_pass.h"
#include "../context.h"
#include "core_debug/debug.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "vector3/vector3.h"

void SkyboxPass::Execute(const Context &_ctx)
{
    static std::shared_ptr<Mesh>     mesh     = FBXAsset::Load("resources/models/cube.fbx")->GetMesh(0);
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("resources/shaders/skybox/skybox.shader", {}));

    if (mesh == nullptr || _ctx.Skybox == nullptr)
        return;

    auto debugGroup = Debug::DebugGroup("Skybox pass");

    glCullFace(GL_FRONT);

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx.ViewMatrix.Invert().GetPosition());

    Shader::SetGlobalMatrix("_ModelMatrix", modelMatrix);
    material->SetTexture("_Skybox", _ctx.Skybox);
    mesh->Draw(*material);

    glCullFace(GL_BACK);
}
