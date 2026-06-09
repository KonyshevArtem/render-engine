#include "shadow_map_debug_pass.h"
#include "graphics/graphics.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "material/material.h"
#include "graphics_backend_api.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics/render_data.h"
#include "developer_console/developer_console.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "resources/resources.h"

bool ShadowMapDebugPass::DrawShadowCascades = false;

ShadowMapDebugPass::ShadowMapDebugPass() : RenderPass()
{
    m_FullscreenMesh = Mesh::GetFullscreenMesh();
    
    DeveloperConsole::AddCommand(L"Shadows.DrawCascades", &DrawShadowCascades);
}

void ShadowMapDebugPass::Prepare(RenderData& renderData)
{
}

void ShadowMapDebugPass::Execute(const RenderData& renderData)
{
    struct DebugData
    {
        Matrix4x4 InvCameraVP;
    };

    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = sizeof(DebugData);

    static std::shared_ptr<Shader> shader = Resources::LoadShader("core_resources/shaders/editor/shadowCascadeVisualize", {});
    static std::shared_ptr<GraphicsBuffer> buffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "ShadowCascadeVisualizeData");

    if (DrawShadowCascades)
    {
        if (!shader || !shader->IsValid())
			return;

        DebugData data{};
        data.InvCameraVP = (renderData.ProjectionMatrix * renderData.ViewMatrix).Invert();

        const GraphicsBackendRenderTargetDescriptor colorTarget{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget.Texture->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
        GraphicsBackend::Current()->AttachRenderTarget(colorTarget);

        GraphicsBackend::Current()->BeginRenderPass("Shadow Cascade Visualize Pass");

        buffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(buffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(renderData.CameraDepthTarget.View->GetBackendTextureView(), renderData.CameraDepthTarget.Texture->GetBackendSampler(), 0);

        GraphicsBackend::Current()->SetBlendState(GraphicsBackendBlendDescriptor::PremultipliedAlphaBlending());
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::AlwaysPassNoWrite());
        GraphicsBackend::Current()->SetRasterizerState(GraphicsBackendRasterizerDescriptor::NoCull());

        GraphicsBackend::Current()->UseProgram(shader->GetProgram(m_FullscreenMesh));
        GraphicsBackend::Current()->DrawElements(m_FullscreenMesh->GetGraphicsBackendGeometry(), m_FullscreenMesh->GetPrimitiveType(), m_FullscreenMesh->GetElementsCount(), m_FullscreenMesh->GetIndicesDataType());

        GraphicsBackend::Current()->EndRenderPass();
    }
}