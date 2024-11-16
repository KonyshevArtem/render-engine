#include "skybox_pass.h"
#include "graphics/context.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug_group.h"
#include "material/material.h"
#include "shader/shader.h"

void SkyboxPass::Execute(const Context &_ctx)
{
    static std::shared_ptr<Mesh> mesh = FBXAsset::Load("core_resources/models/cube.fbx")->GetMesh(0);
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("core_resources/shaders/skybox", {}, {}, {CullFace::FRONT}, {}));

    if (mesh == nullptr || _ctx.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebugGroup("Skybox pass");

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx.ViewMatrix.Invert().GetPosition());
    material->SetTexture("_Skybox", _ctx.Skybox);

    Graphics::Draw(*mesh, *material, modelMatrix, 0);
}
