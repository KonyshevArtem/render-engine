#include "skybox_pass.h"
#include "graphics/context.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug_group.h"
#include "material/material.h"
#include "shader/shader.h"

SkyboxPass::SkyboxPass(int priority) :
    RenderPass(priority)
{
}

void SkyboxPass::Execute(const Context& ctx)
{
    static const std::shared_ptr<Mesh> mesh = FBXAsset::Load("core_resources/models/cube.fbx")->GetMesh(0);
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/skybox", {}, {}, {CullFace::FRONT}, {});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "Skybox");

    if (mesh == nullptr || ctx.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebugGroup("Skybox pass");

    const Matrix4x4 modelMatrix = Matrix4x4::Translation(ctx.ViewMatrix.Invert().GetPosition());
    material->SetTexture("_Skybox", ctx.Skybox);

    Graphics::Draw(*mesh, *material, modelMatrix, 0);
}
