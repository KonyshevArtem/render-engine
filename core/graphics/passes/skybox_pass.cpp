#include "skybox_pass.h"
#include "graphics/context.h"
#include "cubemap/cubemap.h"
#include "fbx_asset/fbx_asset.h"
#include "graphics/graphics.h"
#include "graphics/render_settings.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug.h"
#include "shader/shader.h"

#include <vector>

void SkyboxPass::Execute(const Context &_ctx)
{
    static RenderSettings            renderSettings;
    static std::shared_ptr<Mesh>     mesh     = FBXAsset::Load("resources/models/cube.fbx")->GetMesh(0);
    static std::shared_ptr<Material> material = std::make_shared<Material>(Shader::Load("resources/shaders/skybox/skybox.shader", {}));
    static std::vector<DrawCallInfo> drawCalls(1);

    if (mesh == nullptr || _ctx.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebug::DebugGroup("Skybox pass");

    Matrix4x4 modelMatrix = Matrix4x4::Translation(_ctx.ViewMatrix.Invert().GetPosition());
    material->SetTexture("_Skybox", _ctx.Skybox);

    drawCalls[0] = {mesh, material, modelMatrix};
    Graphics::Draw(drawCalls, renderSettings);
}
