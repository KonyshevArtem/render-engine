#include "skybox_pass.h"
#include "../context.h"
#include "core_debug/debug.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "graphics/draw_call_info.h"
#include "graphics/graphics.h"
#include "graphics/render_settings.h"
#include "material/material.h"
#include "matrix4x4/matrix4x4.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include <vector>

void SkyboxPass::Execute(const Context &_ctx)
{
    static RenderSettings            renderSettings;
    static std::shared_ptr<Mesh>     mesh     = FBXAsset::Load("resources/models/cube.fbx")->GetMesh(0);
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("resources/shaders/skybox/skybox.shader", {}));

    if (mesh == nullptr || _ctx.Skybox == nullptr)
        return;

    auto debugGroup = Debug::DebugGroup("Skybox pass");

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx.ViewMatrix.Invert().GetPosition());
    material->SetTexture("_Skybox", _ctx.Skybox);

    DrawCallInfo info {mesh, material, modelMatrix};
    Graphics::Draw(std::vector<DrawCallInfo> {info}, renderSettings);
}
