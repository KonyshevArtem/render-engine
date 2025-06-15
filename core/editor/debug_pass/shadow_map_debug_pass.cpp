#include "shadow_map_debug_pass.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "material/material.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/graphics_buffer.h"
#include "types/graphics_backend_buffer_info.h"

const bool s_Enabled = false;

ShadowMapDebugPass::ShadowMapDebugPass(int priority) : RenderPass(priority)
{
    m_FullscreenMesh = Mesh::GetFullscreenMesh();
}

void ShadowMapDebugPass::Execute(const Context& ctx)
{
    struct DebugData
    {
        uint32_t LightType;
        uint32_t LightIndex;
        uint32_t PointLightSide;
        float ScreenAspect;

        float Scale;
        float MinDepth;
        float MaxDepth;
        float Padding0;
    };

    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowMapDebug", {}, {}, {CullFace::NONE, CullFaceOrientation::CLOCKWISE}, {false, DepthFunction::ALWAYS});
    static std::shared_ptr<Material> material = std::make_shared<Material>(shader, "ShadowMap Debug");
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(sizeof(DebugData), "ShadowMapDebugData");
    static GraphicsBackendResourceBindings bindings = shader->GetBuffers().at("DebugData")->GetBinding();

    if (s_Enabled)
    {
        DebugData data{};
        data.LightType = 0;
        data.LightIndex = 0;
        data.PointLightSide = 0;
        data.ScreenAspect = static_cast<float>(Graphics::GetScreenWidth()) / static_cast<float>(Graphics::GetScreenHeight());
        data.Scale = 0.4f;
        data.MinDepth = 0.0f;
        data.MaxDepth = 1.0f;

        GraphicsBackend::Current()->BeginRenderPass("ShadowMap Debug Pass");
        buffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), bindings, 0, sizeof(data));
        Graphics::Draw(*m_FullscreenMesh, *material, Matrix4x4::Identity());
        GraphicsBackend::Current()->EndRenderPass();
    }
};