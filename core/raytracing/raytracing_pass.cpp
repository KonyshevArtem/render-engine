#include "raytracing_pass.h"
#include "shader/shader.h"
#include "developer_console/developer_console.h"
#include "graphics_backend_api.h"
#include "raytracing_scene.h"
#include "editor/profiler/profiler.h"
#include "enums/resource_state.h"
#include "graphics/render_data.h"
#include "texture/texture.h"
#include "graphics_buffer/graphics_buffer.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "mesh/mesh.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "global_constants.h"
#include "editor/texture_viewer/texture_viewer.h"
#include "graphics/graphics_settings.h"
#include "resources/resources.h"

RaytracingPass::RaytracingPass(const std::shared_ptr<RaytracingScene>& rtScene) :
	m_PrimaryRaysDebugEnabled(false),
	m_RaytracedShadowsEnabled(true),
	m_RaytracedSoftShadowsEnabled(true),
	m_RaytracingScene(rtScene),
	m_Rng(std::random_device{}())
{
	LoadShaders(false);
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/primary_rays_debug.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytraced_shadows.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_common.h");

	DeveloperConsole::AddCommand(L"Raytracing.Debug.PrimaryRays", &m_PrimaryRaysDebugEnabled);
	DeveloperConsole::AddCommand(L"Raytracing.Shadows.Enabled", &m_RaytracedShadowsEnabled);
	DeveloperConsole::AddCommand(L"Raytracing.Shadows.Soft", &m_RaytracedSoftShadowsEnabled);
	DeveloperConsole::AddCommand(L"Raytracing.Shadows.Samples", &m_RaytracedShadowsSamplesCount);

	m_BlueNoiseTexture.Texture = Resources::Load<Texture>("core_resources/textures/noise/blue_noise");

	GraphicsBackendTextureViewDescriptor noiseTextureViewDescriptor{};
	noiseTextureViewDescriptor.Format = m_BlueNoiseTexture.Texture->GetTextureDescriptor().Format;
	m_BlueNoiseTexture.View = std::make_shared<TextureView>(m_BlueNoiseTexture.Texture, noiseTextureViewDescriptor, "Raytracing/BlueNoiseTextureView");
}

void RaytracingPass::Prepare(RenderData& renderData)
{
	if (m_FileWatcher.FilesChanged())
		LoadShaders(true);

	renderData.RaytracedShadowsEnabled = m_RaytracedShadowsEnabled;
	if (m_RaytracedShadowsEnabled)
	{
		Shader::AddGlobalDefine("_RAYTRACED_SHADOWS");

		const uint32_t width = renderData.CameraDepthTarget.Texture->GetWidth();
		const uint32_t height = renderData.CameraDepthTarget.Texture->GetHeight();
		if (!m_RaytracedShadowsTarget.Texture || m_RaytracedShadowsTarget.Texture->GetWidth() != width || m_RaytracedShadowsTarget.Texture->GetHeight() != height)
		{
			GraphicsBackendTextureDescriptor textureDescriptor{};
			textureDescriptor.Type = TextureType::TEXTURE_2D;
			textureDescriptor.Width = width;
			textureDescriptor.Height = height;
			textureDescriptor.MipLevels = 1;
			textureDescriptor.Format = TextureInternalFormat::R16F;
			textureDescriptor.Linear = true;
			textureDescriptor.RenderTarget = true;

			GraphicsBackendTextureViewDescriptor textureViewDescriptor{};
			textureViewDescriptor.Format = textureDescriptor.Format;

			m_RaytracedShadowsTarget.Texture = std::make_shared<Texture>(textureDescriptor, "Raytracing/RaytracedShadowsTarget");
			m_RaytracedShadowsTarget.View = std::make_shared<TextureView>(m_RaytracedShadowsTarget.Texture, textureViewDescriptor, "Raytracing/RaytracedShadowsTargetView");
		}

		renderData.RaytracedShadowsTarget = m_RaytracedShadowsTarget;
	}
	else
		Shader::RemoveGlobalDefine("_RAYTRACED_SHADOWS");
}

void RaytracingPass::Execute(const RenderData& renderData)
{
}

void RaytracingPass::ExecuteRaytracedShadows(const RenderData& renderData)
{
	if (!m_RaytracedShadowsEnabled)
		return;

	const std::shared_ptr<Shader> shader = m_RaytracedShadowsShaders[m_RaytracedSoftShadowsEnabled ? 1 : 0];
	if (!shader || !shader->IsValid())
		return;

	Profiler::Marker _("RaytracingPass::ExecuteRaytracedShadows");

	GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{};
	colorTargetDescriptor.Texture = renderData.RaytracedShadowsTarget.Texture->GetBackendTexture();
	colorTargetDescriptor.Attachment = FramebufferAttachment::COLOR_ATTACHMENT0;
	colorTargetDescriptor.LoadAction = LoadAction::CLEAR;

	GraphicsBackend::Current()->AttachRenderTarget(colorTargetDescriptor);
	GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

	GraphicsBackend::Current()->BeginRenderPass("RaytracingPass::ExecuteRaytracedShadows");
	{
		Profiler::GPUMarker gpuMarker("RaytracingPass::ExecuteRaytracedShadows");

		struct
		{
			Vector2 InvTargetSize;
			float ShadowsDistance;
			uint32_t SamplesCount;

			Vector2 Random;
			Vector2 Padding0;
		} constants;

		std::uniform_real_distribution<float> dist(0.0f, 1.0f);

		constants.InvTargetSize = Vector2(1.0f / renderData.RaytracedShadowsTarget.Texture->GetWidth(), 1.0f / renderData.RaytracedShadowsTarget.Texture->GetHeight());
		constants.ShadowsDistance = GraphicsSettings::GetShadowDistance();
		constants.SamplesCount = m_RaytracedShadowsSamplesCount;
		constants.Random = Vector2(dist(m_Rng), dist(m_Rng));

		if (!m_RaytracedShadowsDataBuffer)
		{
			GraphicsBackendBufferDescriptor bufferDescriptor{};
			bufferDescriptor.Size = sizeof(constants);
			bufferDescriptor.AllowCPUWrites = true;
			m_RaytracedShadowsDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "Raytracing/PrimaryRaysDebugBuffer", nullptr);
		}

		m_RaytracedShadowsDataBuffer->SetData(&constants, 0, sizeof(constants));

		GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());
		GraphicsBackend::Current()->SetBlendState(GraphicsBackendBlendDescriptor{});

		m_RaytracingScene->BindResources();

		GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget.View->GetBackendTextureView(), 0);
		GraphicsBackend::Current()->BindTexture(renderData.GBuffers[1].View->GetBackendTextureView(), 1);
		GraphicsBackend::Current()->BindTextureSampler(m_BlueNoiseTexture.View->GetBackendTextureView(), m_BlueNoiseTexture.Texture->GetBackendSampler(), 2);
		GraphicsBackend::Current()->BindConstantBuffer(m_RaytracedShadowsDataBuffer->GetBackendBuffer(), 0, 0, sizeof(constants));

		const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
		GraphicsBackend::Current()->UseProgram(shader->GetProgram(fullscreenMesh));
		GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());

		GraphicsBackend::Current()->BindTexture(renderData.RaytracedShadowsTarget.View->GetBackendTextureView(), GlobalConstants::TextureIndex::RT_SHADOW_MASK);
	}
	GraphicsBackend::Current()->EndRenderPass();

	TextureViewer::RegisterTexture(renderData.RaytracedShadowsTarget.View, "Raytracing/Shadows");
}

void RaytracingPass::ExecutePrimaryRaysDebug(const RenderData& renderData)
{
	if (!m_PrimaryRaysDebugEnabled)
		return;

	Profiler::Marker _("RaytracingPass::ExecutePrimaryRaysDebug");

	if (!m_PrimaryRaysDebugShader || !m_PrimaryRaysDebugShader->IsValid())
		return;

	GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{};
	colorTargetDescriptor.Texture = renderData.CameraColorTarget.Texture->GetBackendTexture();
	colorTargetDescriptor.Attachment = FramebufferAttachment::COLOR_ATTACHMENT0;
	colorTargetDescriptor.LoadAction = LoadAction::CLEAR;

	GraphicsBackend::Current()->AttachRenderTarget(colorTargetDescriptor);
	GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

	GraphicsBackend::Current()->BeginRenderPass("RaytracingPass::ExecutePrimaryRaysDebug");
	{
		Profiler::GPUMarker gpuMarker("RaytracingPass::ExecutePrimaryRaysDebug");

		struct
		{
			Vector2UI TargetSize;
			Vector2 Padding0;
		} constants;

		constants.TargetSize = Vector2UI(renderData.CameraColorTarget.Texture->GetWidth(), renderData.CameraColorTarget.Texture->GetHeight());

		if (!m_PrimaryRaysDebugDataBuffer)
		{
			GraphicsBackendBufferDescriptor bufferDescriptor{};
			bufferDescriptor.Size = sizeof(constants);
			bufferDescriptor.AllowCPUWrites = true;
			m_PrimaryRaysDebugDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "Raytracing/PrimaryRaysDebugBuffer", nullptr);
		}

		m_PrimaryRaysDebugDataBuffer->SetData(&constants, 0, sizeof(constants));

		GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());
		GraphicsBackend::Current()->SetBlendState(GraphicsBackendBlendDescriptor{});

		m_RaytracingScene->BindResources();
		GraphicsBackend::Current()->BindConstantBuffer(m_PrimaryRaysDebugDataBuffer->GetBackendBuffer(), 0, 0, sizeof(constants));

		const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
		GraphicsBackend::Current()->UseProgram(m_PrimaryRaysDebugShader->GetProgram(fullscreenMesh));
		GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());

	}
	GraphicsBackend::Current()->EndRenderPass();
}

void RaytracingPass::LoadShaders(bool reload)
{
	m_PrimaryRaysDebugShader = Resources::LoadShader("core_resources/shaders/raytracing/primary_rays_debug", { "_RECEIVE_SHADOWS" }, reload);
	m_RaytracedShadowsShaders[0] = Resources::LoadShader("core_resources/shaders/raytracing/raytraced_shadows", {}, reload);
	m_RaytracedShadowsShaders[1] = Resources::LoadShader("core_resources/shaders/raytracing/raytraced_shadows", {"RAYTRACED_SOFT_SHADOWS"}, reload);
}
