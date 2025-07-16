#include "skybox_pass.h"
#include "graphics/context.h"
#include "cubemap/cubemap.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "graphics_backend_debug_group.h"
#include "material/material.h"
#include "shader/shader.h"
#include "resources/resources.h"

std::shared_ptr<Mesh> SkyboxPass::m_Mesh = nullptr;

SkyboxPass::SkyboxPass(int priority) :
    RenderPass(priority)
{
}

void SkyboxPass::Prepare()
{
    if (!m_Mesh)
        m_Mesh = Resources::Load<Mesh>("core_resources/models/Cube");
}

void SkyboxPass::Execute(const Context& ctx)
{
    static const std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/skybox", {}, {}, {CullFace::FRONT}, {});
    static const std::shared_ptr<Material> material = std::make_shared<Material>(shader, "Skybox");

    if (m_Mesh == nullptr || ctx.Skybox == nullptr)
        return;

    auto debugGroup = GraphicsBackendDebugGroup("Skybox pass", GPUQueue::RENDER);

    const Matrix4x4 modelMatrix = Matrix4x4::Translation(ctx.ViewMatrix.Invert().GetPosition());
    Graphics::Draw(*m_Mesh, *material, modelMatrix);
}
