#include "deferred_light_pass.h"
#include "graphics/render_data.h"
#include "texture_2d/texture_2d.h"
#include "editor/profiler/profiler.h"
#include "mesh/mesh.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_buffer_descriptor.h"

DeferredLightPass::DeferredLightPass()
{
	m_LightShader = Shader::Load("core_resources/shaders/deferred_light", {"_REFLECTION", "_RECEIVE_SHADOWS"});
}

void DeferredLightPass::Prepare(RenderData& renderData)
{
	Profiler::Marker marker("DeferredLightPass::Prepare");

    const uint32_t width = renderData.Viewport.x;
    const uint32_t height = renderData.Viewport.y;
    if (m_CameraColorTarget == nullptr || m_CameraColorTarget->GetWidth() != width || m_CameraColorTarget->GetHeight() != height)
    {
        GraphicsBackendTextureDescriptor descriptor;
        descriptor.Width = width;
        descriptor.Height = height;
        descriptor.Linear = true;
        descriptor.RenderTarget = true;

        descriptor.Format = TextureInternalFormat::RGBA16F;
        m_CameraColorTarget = Texture2D::Create(descriptor, "CameraColorRT");
    }

    renderData.CameraColorTarget = m_CameraColorTarget;
}

void DeferredLightPass::Execute(const RenderData& renderData)
{
	Profiler::Marker marker("DeferredLightPass::Execute");

    struct
    {
		Matrix4x4 InvCameraVP;

        Vector2 InvTargetSize;
        Vector2 Padding;
    } constants{};

    if (!m_LightingDataBuffer)
    {
        GraphicsBackendBufferDescriptor descriptor{};
        descriptor.AllowCPUWrites = true;
        descriptor.Size = sizeof(constants);

        m_LightingDataBuffer = std::make_shared<GraphicsBuffer>(descriptor, "DeferredLight/Data");
    }

	constants.InvCameraVP = (renderData.ProjectionMatrix * renderData.ViewMatrix).Invert();
	constants.InvTargetSize = Vector2(1.0f / renderData.CameraColorTarget->GetWidth(), 1.0f / renderData.CameraColorTarget->GetHeight());
    m_LightingDataBuffer->SetData(&constants, 0, sizeof(constants));

    const GraphicsBackendRenderTargetDescriptor colorDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

    GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
    GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

    GraphicsBackend::Current()->BeginRenderPass("Deferred Light Pass");
	{
		Profiler::GPUMarker gpuMarker("DeferredLightPass::Render", GPUQueue::RENDER);

        GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());

        for (int i = 0; i < 2; i++)
	        GraphicsBackend::Current()->BindTexture(renderData.GBuffers[i]->GetBackendTexture(), i);
        GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget->GetBackendTexture(), 2);
		GraphicsBackend::Current()->BindConstantBuffer(m_LightingDataBuffer->GetBackendBuffer(), 0, 0, sizeof(constants));

		const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
        GraphicsBackend::Current()->UseProgram(m_LightShader->GetProgram(fullscreenMesh));
        GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
	}
	GraphicsBackend::Current()->EndRenderPass();
}
