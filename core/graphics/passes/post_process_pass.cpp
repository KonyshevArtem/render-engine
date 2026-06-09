#include "post_process_pass.h"
#include "editor/profiler/profiler.h"
#include "texture_2d/texture_2d.h"
#include "material/material.h"
#include "shader/shader.h"
#include "graphics/graphics.h"
#include "graphics/graphics_settings.h"
#include "graphics/render_data.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "resources/resources.h"

namespace PostProcessPass_Local
{
    struct Data
    {
        float OneOverGamma;
        float Exposure;
        uint32_t TonemappingMode;
        float Padding0;
    };
}

PostProcessPass::PostProcessPass() :
    RenderPass()
{
    GraphicsBackendBufferDescriptor bufferDescriptor{};
    bufferDescriptor.AllowCPUWrites = true;
    bufferDescriptor.Size = sizeof(PostProcessPass_Local::Data);

    m_PostProcessShader = Resources::LoadShader("core_resources/shaders/post_process", {});
    m_PostProcessDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "Post Process Data");
}

void PostProcessPass::Prepare(RenderData& renderData)
{
	const uint32_t width = renderData.CameraColorTarget.Texture->GetWidth();
	const uint32_t height = renderData.CameraColorTarget.Texture->GetHeight();
    if (!m_PostProcessedTarget.Texture || m_PostProcessedTarget.Texture->GetWidth() != width || m_PostProcessedTarget.Texture->GetHeight() != height)
    {
        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Format = TextureInternalFormat::RGBA8;
        descriptor.Width = width;
        descriptor.Height = height;
        descriptor.Linear = true;
        descriptor.RenderTarget = true;

		GraphicsBackendTextureViewDescriptor viewDescriptor{};
		viewDescriptor.Format = descriptor.Format;

        m_PostProcessedTarget.Texture = Texture2D::Create(descriptor, "PostProcessedRT");
		m_PostProcessedTarget.View = std::make_shared<TextureView>(m_PostProcessedTarget.Texture, viewDescriptor, "PostProcessedRT_View");
    }

    renderData.PostProcessedTarget = m_PostProcessedTarget;
}

void PostProcessPass::Execute(const RenderData& renderData)
{
    Profiler::Marker marker("PostProcessPass::Execute");

    if (!m_PostProcessShader || !m_PostProcessShader->IsValid())
		return;

    PostProcessPass_Local::Data data{};
    data.OneOverGamma = 1 / GraphicsSettings::GetGamma();
    data.Exposure = GraphicsSettings::GetExposure();
    data.TonemappingMode = static_cast<uint32_t>(GraphicsSettings::GetTonemappingMode());

    GraphicsBackendRenderTargetDescriptor rtDesc{};
    rtDesc.Attachment = FramebufferAttachment::COLOR_ATTACHMENT0;
    rtDesc.Texture = m_PostProcessedTarget.Texture->GetBackendTexture();
    rtDesc.LoadAction = LoadAction::DONT_CARE;

    GraphicsBackend::Current()->AttachRenderTarget(rtDesc);
    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    GraphicsBackend::Current()->BeginRenderPass("Post Process Pass");
    {
        Profiler::GPUMarker gpuMarker("PostProcessPass::Execute");

        m_PostProcessDataBuffer->SetData(&data, 0, sizeof(data));
        GraphicsBackend::Current()->BindConstantBuffer(m_PostProcessDataBuffer->GetBackendBuffer(), 0, 0, sizeof(data));
        GraphicsBackend::Current()->BindTextureSampler(renderData.CameraColorTarget.View->GetBackendTextureView(), renderData.CameraColorTarget.Texture->GetBackendSampler(), 0);
        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());

        const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
        GraphicsBackend::Current()->UseProgram(m_PostProcessShader->GetProgram(fullscreenMesh));
        GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
    }
    GraphicsBackend::Current()->EndRenderPass();
}
