#include "shadow_map_debug_pass.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "material/material.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/graphics_buffer.h"
#include "types/graphics_backend_buffer_info.h"
#include "graphics/context.h"
#include "types/graphics_backend_sampler_info.h"
#include "developer_console/developer_console.h"

bool ShadowMapDebugPass::DrawShadowMapOverlay = false;
bool ShadowMapDebugPass::DrawShadowCascades = false;

ShadowMapDebugPass::ShadowMapDebugPass(int priority) : RenderPass(priority)
{
    m_FullscreenMesh = Mesh::GetFullscreenMesh();

    DeveloperConsole::AddBoolCommand("Shadows.DrawOverlay", &DrawShadowMapOverlay);
    DeveloperConsole::AddBoolCommand("Shadows.DrawCascades", &DrawShadowCascades);
}

void ShadowMapDebugPass::Prepare(const std::shared_ptr<Texture2D>& depthMap)
{
    m_DepthMap = depthMap;
}

void ShadowMapDebugPass::DrawCascades(const Context& ctx)
{
    struct DebugData
    {
        Matrix4x4 InvCameraVP;
    };

    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowCascadeVisualize", {});
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(sizeof(DebugData), "ShadowCascadeVisualizeData");

    if (DrawShadowCascades)
    {
        DebugData data{};
        data.InvCameraVP = (ctx.ProjectionMatrix * ctx.ViewMatrix).Invert();

        GraphicsBackend::Current()->BeginRenderPass("Shadow Cascade Visualize Pass");
        buffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(m_DepthMap->GetBackendTexture(), m_DepthMap->GetBackendSampler(), 0);
        GraphicsBackend::Current()->SetBlendState(GraphicsBackendBlendDescriptor::PremultipliedAlphaBlending());
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::AlwaysPassNoWrite());
        GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::NoCull());
        GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_FullscreenMesh));
        GraphicsBackend::Current()->DrawElements(m_FullscreenMesh->GetGraphicsBackendGeometry(), m_FullscreenMesh->GetPrimitiveType(), m_FullscreenMesh->GetElementsCount(), m_FullscreenMesh->GetIndicesDataType());
        GraphicsBackend::Current()->EndRenderPass();
    }
}

void ShadowMapDebugPass::DrawOverlay(const Context& ctx)
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

    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowMapOverlay", {});
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(sizeof(DebugData), "ShadowMapOverlayData");
    static GraphicsBackendSampler sampler = GraphicsBackend::Current()->CreateSampler(TextureWrapMode::CLAMP_TO_EDGE, TextureFilteringMode::NEAREST, nullptr, 0, ComparisonFunction::NONE, "ShadowMap Overlay Sampler");

    if (DrawShadowMapOverlay)
    {
        DebugData data{};
        data.LightType = 0;
        data.LightIndex = 0;
        data.PointLightSide = 0;
        data.ScreenAspect = static_cast<float>(Graphics::GetScreenWidth()) / static_cast<float>(Graphics::GetScreenHeight());
        data.Scale = 0.4f;
        data.MinDepth = 0.0f;
        data.MaxDepth = 1.0f;

        GraphicsBackend::Current()->BeginRenderPass("ShadowMap Overlay Pass");
        buffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindSampler(sampler, 0);
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::AlwaysPassNoWrite());
        GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::NoCull());
        GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_FullscreenMesh));
        GraphicsBackend::Current()->DrawElements(m_FullscreenMesh->GetGraphicsBackendGeometry(), m_FullscreenMesh->GetPrimitiveType(), m_FullscreenMesh->GetElementsCount(), m_FullscreenMesh->GetIndicesDataType());
        GraphicsBackend::Current()->EndRenderPass();
    }
}

void ShadowMapDebugPass::Execute(const Context& ctx)
{
    DrawCascades(ctx);
    DrawOverlay(ctx);
}