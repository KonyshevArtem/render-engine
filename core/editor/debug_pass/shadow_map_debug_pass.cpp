#include "shadow_map_debug_pass.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "material/material.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/graphics_buffer.h"
#include "types/graphics_backend_buffer_info.h"
#include "graphics/render_data.h"
#include "types/graphics_backend_sampler_info.h"
#include "developer_console/developer_console.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "types/graphics_backend_buffer_view.h"
#include "types/graphics_backend_render_target_descriptor.h"

bool ShadowMapDebugPass::DrawShadowMapOverlay = false;
bool ShadowMapDebugPass::DrawShadowCascades = false;

ShadowMapDebugPass::ShadowMapDebugPass(int priority) : RenderPass(priority)
{
    m_FullscreenMesh = Mesh::GetFullscreenMesh();

    DeveloperConsole::AddBoolCommand(L"Shadows.DrawOverlay", &DrawShadowMapOverlay);
    DeveloperConsole::AddBoolCommand(L"Shadows.DrawCascades", &DrawShadowCascades);
}

void ShadowMapDebugPass::Prepare(const RenderData& renderData)
{
}

void ShadowMapDebugPass::DrawCascades(const RenderData& renderData) const
{
    struct DebugData
    {
        Matrix4x4 InvCameraVP;
    };

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = sizeof(DebugData);

    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowCascadeVisualize", {});
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "ShadowCascadeVisualizeData");

    if (DrawShadowCascades)
    {
        DebugData data{};
        data.InvCameraVP = (renderData.ProjectionMatrix * renderData.ViewMatrix).Invert();

        const GraphicsBackendRenderTargetDescriptor colorTarget{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
        GraphicsBackend::Current()->AttachRenderTarget(colorTarget);

        GraphicsBackend::Current()->BeginRenderPass("Shadow Cascade Visualize Pass");

        buffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(renderData.CameraDepthTarget->GetBackendTexture(), renderData.CameraDepthTarget->GetBackendSampler(), 0);

        GraphicsBackend::Current()->SetBlendState(GraphicsBackendBlendDescriptor::PremultipliedAlphaBlending());
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::AlwaysPassNoWrite());
        GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::NoCull());

        GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_FullscreenMesh));
        GraphicsBackend::Current()->DrawElements(m_FullscreenMesh->GetGraphicsBackendGeometry(), m_FullscreenMesh->GetPrimitiveType(), m_FullscreenMesh->GetElementsCount(), m_FullscreenMesh->GetIndicesDataType());

        GraphicsBackend::Current()->EndRenderPass();
    }
}

void ShadowMapDebugPass::DrawOverlay(const RenderData& renderData) const
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

    GraphicsBackendSamplerDescriptor samplerDescriptor{};
    samplerDescriptor.FilteringMode = TextureFilteringMode::NEAREST;

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = sizeof(DebugData);

    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/editor/shadowMapOverlay", {});
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "ShadowMapOverlayData");
    static GraphicsBackendSampler sampler = GraphicsBackend::Current()->CreateSampler(samplerDescriptor, "ShadowMap Overlay Sampler");

    if (DrawShadowMapOverlay)
    {
        DebugData data{};
        data.LightType = 0;
        data.LightIndex = 0;
        data.PointLightSide = 0;
        data.ScreenAspect = renderData.Viewport.x / renderData.Viewport.y;
        data.Scale = 0.4f;
        data.MinDepth = 0.0f;
        data.MaxDepth = 1.0f;

        const GraphicsBackendRenderTargetDescriptor colorTarget{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
        GraphicsBackend::Current()->AttachRenderTarget(colorTarget);

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

void ShadowMapDebugPass::Execute(const RenderData& renderData)
{
    DrawCascades(renderData);
    DrawOverlay(renderData);
}