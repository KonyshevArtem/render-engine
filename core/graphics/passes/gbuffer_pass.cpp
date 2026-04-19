#include "gbuffer_pass.h"
#include "graphics/render_data.h"
#include "texture_2d/texture_2d.h"
#include "types/graphics_backend_texture_descriptor.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics/render_settings/render_settings.h"
#include "graphics/graphics.h"
#include "editor/profiler/profiler.h"

void GBufferPass::Prepare(RenderData& renderData)
{
	Profiler::Marker marker("GBufferPass::Prepare");

	const uint32_t width = renderData.Viewport.x;
	const uint32_t height = renderData.Viewport.y;
	if (!m_GBuffers[0] || m_GBuffers[0]->GetWidth() != width || m_GBuffers[0]->GetHeight() != height)
	{
		GraphicsBackendTextureDescriptor gBufferDescriptor{};
		gBufferDescriptor.Format = TextureInternalFormat::RGBA16F;
		gBufferDescriptor.Width = width;
		gBufferDescriptor.Height = height;
		gBufferDescriptor.RenderTarget = true;
		gBufferDescriptor.Linear = true;

		for (int i = 0; i < 2; i++)
			m_GBuffers[i] = Texture2D::Create(gBufferDescriptor, "GBuffer_" + std::to_string(i));
	}

	if (m_CameraDepthTarget == nullptr || m_CameraDepthTarget->GetWidth() != width || m_CameraDepthTarget->GetHeight() != height)
	{
		const TextureInternalFormat depthFormat = GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL ? TextureInternalFormat::DEPTH_32_STENCIL_8 : TextureInternalFormat::DEPTH_24_STENCIL_8;

		GraphicsBackendTextureDescriptor descriptor;
		descriptor.Width = width;
		descriptor.Height = height;
		descriptor.Linear = true;
		descriptor.RenderTarget = true;

		descriptor.Format = depthFormat;
		m_CameraDepthTarget = Texture2D::Create(descriptor, "CameraDepthRT");
	}

	for (int i = 0; i < 2; i++)
		renderData.GBuffers[i] = m_GBuffers[i];
	renderData.CameraDepthTarget = m_CameraDepthTarget;

	RenderSettings renderSettings{};
	renderSettings.Sorting = DrawCallSortMode::FRONT_TO_BACK;
	renderSettings.Filter = DrawCallFilter::Opaque();
	m_RenderQueue.Prepare(renderData.ProjectionMatrix * renderData.ViewMatrix, renderData.Renderers, renderSettings);
}

void GBufferPass::Execute(const RenderData& renderData)
{
	Profiler::Marker marker("GBufferPass::Execute");

	const GraphicsBackendRenderTargetDescriptor gBuffer0Descriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.GBuffers[0]->GetBackendTexture(), .LoadAction = LoadAction::CLEAR};
	const GraphicsBackendRenderTargetDescriptor gBuffer1Descriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT1, .Texture = renderData.GBuffers[1]->GetBackendTexture(), .LoadAction = LoadAction::CLEAR};
	const GraphicsBackendRenderTargetDescriptor depthDescriptor{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

	GraphicsBackend::Current()->AttachRenderTarget(gBuffer0Descriptor);
	GraphicsBackend::Current()->AttachRenderTarget(gBuffer1Descriptor);
	GraphicsBackend::Current()->AttachRenderTarget(depthDescriptor);

	Graphics::SetCameraData(renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.NearPlane, renderData.FarPlane);

	GraphicsBackend::Current()->BeginRenderPass("GBufferPass");
	{
		Profiler::GPUMarker gpuMarker("GBufferPass::Execute");

		GraphicsBackend::Current()->SetViewport(0, 0, renderData.Viewport.x, renderData.Viewport.y, 0, 1);
		GraphicsBackend::Current()->SetScissorRect(0, 0, renderData.Viewport.x, renderData.Viewport.y);

		m_RenderQueue.Draw();
	}
	GraphicsBackend::Current()->EndRenderPass();
}
