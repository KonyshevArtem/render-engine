#include "shadow_map_debug_pass.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "material/material.h"
#include "graphics_backend_api.h"

const bool s_Enabled = false;

ShadowMapDebugPass::ShadowMapDebugPass(int priority) : RenderPass(priority)
{
    m_FullscreenMesh = Mesh::GetFullscreenMesh();
}

void ShadowMapDebugPass::Execute(const Context& ctx)
{
    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowMapDebug", {}, {}, {CullFace::NONE, CullFaceOrientation::CLOCKWISE}, {false, DepthFunction::ALWAYS});
    static std::shared_ptr<Material> material = std::make_shared<Material>(shader, "ShadowMap Debug");

    if (s_Enabled)
    {
        GraphicsBackend::Current()->BeginRenderPass("ShadowMap Debug Pass");
        Graphics::Draw(*m_FullscreenMesh, *material, Matrix4x4::Identity());
        GraphicsBackend::Current()->EndRenderPass();
    }
};