#include "gbuffer_pass.h"
#include "graphics/render_data.h"
#include "texture/texture.h"
#include "types/graphics_backend_texture_descriptor.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics/render_settings/render_settings.h"
#include "graphics/graphics.h"
#include "editor/profiler/profiler.h"
#include "editor/texture_viewer/texture_viewer.h"

void GBufferPass::Prepare(RenderData& renderData)
{
	Profiler::Marker marker("GBufferPass::Prepare");

	const uint32_t width = renderData.Viewport.x;
	const uint32_t height = renderData.Viewport.y;
	if (!m_GBuffers[0].Texture || m_GBuffers[0].Texture->GetWidth() != width || m_GBuffers[0].Texture->GetHeight() != height)
	{
		GraphicsBackendTextureDescriptor gBufferDescriptor{};
		gBufferDescriptor.Type = TextureType::TEXTURE_2D;
		gBufferDescriptor.Format = TextureInternalFormat::RGBA16F;
		gBufferDescriptor.Width = width;
		gBufferDescriptor.Height = height;
		gBufferDescriptor.RenderTarget = true;
		gBufferDescriptor.Linear = true;

		GraphicsBackendTextureViewDescriptor gBufferViewDescriptor{};
		gBufferViewDescriptor.Format = gBufferDescriptor.Format;

		for (int i = 0; i < 2; i++)
		{
			m_GBuffers[i].Texture = std::make_shared<Texture>(gBufferDescriptor, "GBuffer_" + std::to_string(i));
			m_GBuffers[i].View = std::make_shared<TextureView>(m_GBuffers[i].Texture, gBufferViewDescriptor, "GBufferView_" + std::to_string(i));
		}
	}

	if (!m_CameraDepthTarget.Texture || m_CameraDepthTarget.Texture->GetWidth() != width || m_CameraDepthTarget.Texture->GetHeight() != height)
	{
		const TextureInternalFormat depthFormat = GraphicsBackend::Current()->GetName() == GraphicsBackendName::METAL ? TextureInternalFormat::DEPTH_32_STENCIL_8 : TextureInternalFormat::DEPTH_24_STENCIL_8;

		GraphicsBackendTextureDescriptor descriptor{};
		descriptor.Type = TextureType::TEXTURE_2D;
		descriptor.Format = depthFormat;
		descriptor.Width = width;
		descriptor.Height = height;
		descriptor.Linear = true;
		descriptor.RenderTarget = true;

		GraphicsBackendTextureViewDescriptor viewDescriptor{};
		viewDescriptor.Format = depthFormat;

		m_CameraDepthTarget.Texture = std::make_shared<Texture>(descriptor, "CameraDepthRT");
		m_CameraDepthTarget.View = std::make_shared<TextureView>(m_CameraDepthTarget.Texture, viewDescriptor, "CameraDepthRT_View");
	}

	for (int i = 0; i < 2; i++)
		renderData.GBuffers[i] = m_GBuffers[i];
	renderData.CameraDepthTarget = m_CameraDepthTarget;

	RenderSettings renderSettings{};
	renderSettings.Sorting = DrawCallSortMode::MATERIAL;
	renderSettings.Filter = DrawCallFilter::Opaque();
	m_RenderQueue.Prepare(renderData.ProjectionMatrix * renderData.ViewMatrix, renderData.Renderers, renderSettings);
}

void GBufferPass::Execute(const RenderData& renderData)
{
	Profiler::Marker marker("GBufferPass::Execute");

	const GraphicsBackendRenderTargetDescriptor gBuffer0Descriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.GBuffers[0].Texture->GetBackendTexture(), .LoadAction = LoadAction::CLEAR};
	const GraphicsBackendRenderTargetDescriptor gBuffer1Descriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT1, .Texture = renderData.GBuffers[1].Texture->GetBackendTexture(), .LoadAction = LoadAction::CLEAR};
	const GraphicsBackendRenderTargetDescriptor depthDescriptor{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget.Texture->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

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

	for (int i = 0; i < 2; ++i)
		TextureViewer::RegisterTexture(renderData.GBuffers[i].View, "GBuffer/" + std::to_string(i));
	TextureViewer::RegisterTexture(renderData.CameraDepthTarget.View, "GBuffer/Depth");
}
