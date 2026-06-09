#include "raytracing_probes.h"
#include "developer_console/developer_console.h"
#include "resources/resources.h"
#include "editor/profiler/profiler.h"
#include "graphics_backend_api.h"
#include "raytracing_scene.h"
#include "enums/load_action.h"
#include "graphics/graphics.h"
#include "graphics/render_data.h"
#include "texture/texture.h"
#include "texture_2d/texture_2d.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "graphics_buffer/graphics_buffer.h"
#include "mesh/mesh.h"
#include "editor/texture_viewer/texture_viewer.h"
#include "input/input.h"
#include "global_constants.h"

namespace RaytracingProbesLocal
{
	constexpr uint32_t k_ProbesPerAtlasRow = 128;
	constexpr uint32_t k_ProbesBorderSize = 2;
}

RaytracingProbes::RaytracingProbes(const std::shared_ptr<RaytracingScene>& rtScene) :
	m_RaytracingProbesEnabled(true),
	m_DebugDrawProbes(false),
	m_DebugDrawProbeGI(false),
	m_ProbeSpacing(2.0f),
	m_ProbesGridSize(32, 8, 32),
	m_ProbesPerUpdate(1024),
	m_UpdateProbeBaseIndex(0),
	m_ProbeLightSize(8),
	m_ProbeLightPadding(1),
	m_RaytracingScene(rtScene)
{
	DeveloperConsole::AddCommand(L"Raytracing.Probes.Enabled", &m_RaytracingProbesEnabled);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.Debug.DrawProbes", &m_DebugDrawProbes);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.Debug.DrawGI", &m_DebugDrawProbeGI);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.Spacing", &m_ProbeSpacing);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.GridSize.X", &m_ProbesGridSize.x);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.GridSize.Y", &m_ProbesGridSize.y);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.GridSize.Z", &m_ProbesGridSize.z);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.UpdatePerFrame", &m_ProbesPerUpdate);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.LightSize", &m_ProbeLightSize);
	DeveloperConsole::AddCommand(L"Raytracing.Probes.LightPadding", &m_ProbeLightPadding);

	m_SphereMesh = Resources::Load<Mesh>("core_resources/models/sphere");

	GraphicsBackendBufferDescriptor bufferDescriptor{};
	bufferDescriptor.Size = sizeof(ProbesData);
	bufferDescriptor.AllowCPUWrites = true;
	m_ProbesDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "RaytracingProbesData");

	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_probe_trace.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_probe_integrate.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_probe_add_border.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_debug_draw_probes.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/raytracing_debug_draw_probe_gi.hlsl");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/probes_common.h");
	m_FileWatcher.AddFile("core_resources/shaders/raytracing/probes_sample.h");
	m_FileWatcher.AddFile("core_resources/shaders/common/lighting.h");

	LoadShaders(false);
}

void RaytracingProbes::Prepare(RenderData& renderData)
{
	if (!m_RaytracingProbesEnabled)
	{
		Shader::RemoveGlobalDefine("PROBE_GI");

		m_ProbeLightAtlas.Clear();
		m_ProbeDepthAtlas.Clear();
		m_ProbeTempLightAtlas.Clear();
		m_ProbeTempDepthAtlas.Clear();
		return;
	}

	Shader::AddGlobalDefine("PROBE_GI");

	Profiler::Marker _("RaytracingProbes::Prepare");

	if (m_FileWatcher.FilesChanged())
		LoadShaders(true);

	const uint32_t probesCount = m_ProbesGridSize.x * m_ProbesGridSize.y * m_ProbesGridSize.z;
	const uint32_t paddedLightSize = m_ProbeLightSize + m_ProbeLightPadding + RaytracingProbesLocal::k_ProbesBorderSize;

	const uint32_t atlasWidth = RaytracingProbesLocal::k_ProbesPerAtlasRow * paddedLightSize;
	const uint32_t atlasHeight = std::max<uint32_t>(probesCount / RaytracingProbesLocal::k_ProbesPerAtlasRow, 1) * paddedLightSize;
	const uint32_t tempAtlasWidth = std::min<uint32_t>(m_ProbesPerUpdate, RaytracingProbesLocal::k_ProbesPerAtlasRow) * paddedLightSize;
	const uint32_t tempAtlasHeight = std::max<uint32_t>(m_ProbesPerUpdate / RaytracingProbesLocal::k_ProbesPerAtlasRow, 1) * paddedLightSize;

	UpdateTextureResources(m_ProbeLightAtlas, atlasWidth, atlasHeight, TextureInternalFormat::RGBA16F, "ProbeLightAtlas");
	UpdateTextureResources(m_ProbeDepthAtlas, atlasWidth, atlasHeight, TextureInternalFormat::RG16F, "ProbeDepthAtlas");
	UpdateTextureResources(m_ProbeTempLightAtlas, tempAtlasWidth, tempAtlasHeight, TextureInternalFormat::RGBA16F, "ProbeTempLightAtlas");
	UpdateTextureResources(m_ProbeTempDepthAtlas, tempAtlasWidth, tempAtlasHeight, TextureInternalFormat::R16F, "ProbeTempDepthAtlas");
}

void RaytracingProbes::Execute(const RenderData& renderData)
{
	if (!m_RaytracingProbesEnabled || !m_ProbeTraceShader || !m_ProbeTraceShader->IsValid() || !m_ProbeIntegrateShader || !m_ProbeIntegrateShader->IsValid())
		return;

	Profiler::Marker _("RaytracingProbes::Execute");

	GraphicsBackend::Current()->BeginComputePass("RaytracingProbes::Execute");
	{
		Profiler::GPUMarker gpuMarker("RaytracingProbes::Execute");

		m_RaytracingScene->BindResources();
		UpdateProbesData();

		BindResources();

		// Trace
		GraphicsBackend::Current()->BindRWTexture(m_ProbeTempLightAtlas.RWView->GetBackendTextureView(), 0);
		GraphicsBackend::Current()->BindRWTexture(m_ProbeTempDepthAtlas.RWView->GetBackendTextureView(), 1);
		GraphicsBackend::Current()->UseProgram(m_ProbeTraceShader->GetProgram());
		GraphicsBackend::Current()->Dispatch(m_ProbeLightSize * m_ProbesPerUpdate, m_ProbeLightSize, 1);

		// Integrate
		GraphicsBackend::Current()->BindTexture(m_ProbeTempLightAtlas.View->GetBackendTextureView(), 0);
		GraphicsBackend::Current()->BindTexture(m_ProbeTempDepthAtlas.View->GetBackendTextureView(), 1);
		GraphicsBackend::Current()->BindRWTexture(m_ProbeLightAtlas.RWView->GetBackendTextureView(), 0);
		GraphicsBackend::Current()->BindRWTexture(m_ProbeDepthAtlas.RWView->GetBackendTextureView(), 1);
		GraphicsBackend::Current()->UseProgram(m_ProbeIntegrateShader->GetProgram());
		GraphicsBackend::Current()->Dispatch(m_ProbeLightSize * m_ProbesPerUpdate, m_ProbeLightSize, 1);

		// Add border
		GraphicsBackend::Current()->UseProgram(m_ProbeAddBorderShader->GetProgram());
		GraphicsBackend::Current()->Dispatch(m_ProbesPerUpdate, 1, 1);
	}
	GraphicsBackend::Current()->EndComputePass();

	const uint32_t probesCount = m_ProbesGridSize.x * m_ProbesGridSize.y * m_ProbesGridSize.z;
	m_UpdateProbeBaseIndex += m_ProbesPerUpdate;
	if (m_UpdateProbeBaseIndex >= probesCount)
		m_UpdateProbeBaseIndex = 0;

	TextureViewer::RegisterTexture(m_ProbeTempLightAtlas.View, "Raytracing/ProbeTempLightAtlas");
	TextureViewer::RegisterTexture(m_ProbeTempDepthAtlas.View, "Raytracing/ProbeTempDepthAtlas");
	TextureViewer::RegisterTexture(m_ProbeLightAtlas.View, "Raytracing/ProbeLightAtlas");
	TextureViewer::RegisterTexture(m_ProbeDepthAtlas.View, "Raytracing/ProbeDepthAtlas");
}

void RaytracingProbes::ExecuteDebug(const RenderData& renderData)
{
	if (!m_RaytracingProbesEnabled)
		return;

	Profiler::Marker _("RaytracingProbes::ExecuteDebug");

	Graphics::SetCameraData(renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.NearPlane, renderData.FarPlane);

	struct DebugData
	{
		Vector2I MouseCoord;
		Vector2 InvTargetSize;
	} debugData;

	if (!m_ProbesDebugDataBuffer)
	{
		GraphicsBackendBufferDescriptor descriptor{};
		descriptor.Size = sizeof(debugData);
		descriptor.AllowCPUWrites = true;

		m_ProbesDebugDataBuffer = std::make_shared<GraphicsBuffer>(descriptor, "RaytracingProbesDebugData");
	}

	debugData.MouseCoord = Vector2I(Input::GetMousePosition().x, Input::GetMousePosition().y);
	debugData.InvTargetSize = Vector2(1.0f / renderData.CameraDepthTarget.Texture->GetWidth(), 1.0f / renderData.CameraDepthTarget.Texture->GetHeight());
	m_ProbesDebugDataBuffer->SetData(&debugData, 0, sizeof(debugData));

	UpdateProbesData();

	if (m_DebugDrawProbes && m_DebugDrawProbesShader && m_DebugDrawProbesShader->IsValid())
	{
		Profiler::Marker _("RaytracingProbes::DebugDrawProbes");

		const GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget.Texture->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
		const GraphicsBackendRenderTargetDescriptor depthTargetDescriptor{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget.Texture->GetBackendTexture(), .LoadAction = LoadAction::LOAD };

		GraphicsBackend::Current()->AttachRenderTarget(colorTargetDescriptor);
		GraphicsBackend::Current()->AttachRenderTarget(depthTargetDescriptor);

		GraphicsBackend::Current()->BeginRenderPass("RaytracingProbes::DebugDrawProbes");
		{
			Profiler::GPUMarker gpuMarker("RaytracingProbes::DebugDrawProbes");

			BindResources();
			GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDebugDataBuffer->GetBackendBuffer(), 1, 0, sizeof(debugData));
			GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget.View->GetBackendTextureView(), 1);

			const uint32_t probesCount = m_ProbesGridSize.x * m_ProbesGridSize.y * m_ProbesGridSize.z;

			GraphicsBackend::Current()->UseProgram(m_DebugDrawProbesShader->GetProgram(m_SphereMesh));
			GraphicsBackend::Current()->DrawElementsInstanced(m_SphereMesh->GetGraphicsBackendGeometry(), m_SphereMesh->GetPrimitiveType(), m_SphereMesh->GetIndexCount(), m_SphereMesh->GetIndicesDataType(), probesCount);
		}
		GraphicsBackend::Current()->EndRenderPass();
	}

	if (m_DebugDrawProbeGI && m_DebugDrawProbeGIShader && m_DebugDrawProbeGIShader->IsValid())
	{
		Profiler::Marker _("RaytracingProbes::DebugDrawProbeGI");

		const uint32_t width = renderData.Viewport.x;
		const uint32_t height = renderData.Viewport.y;
		if (!m_DebugProbeGITarget.Texture || m_DebugProbeGITarget.Texture->GetWidth() != width || m_DebugProbeGITarget.Texture->GetHeight() != height)
		{
			GraphicsBackendTextureDescriptor descriptor;
			descriptor.Format = TextureInternalFormat::RGBA16F;
			descriptor.Width = width;
			descriptor.Height = height;
			descriptor.Linear = true;
			descriptor.RenderTarget = true;

			GraphicsBackendTextureViewDescriptor viewDescriptor;
			viewDescriptor.Format = descriptor.Format;

			m_DebugProbeGITarget.Texture = Texture2D::Create(descriptor, "DebugProbeGITarget");
			m_DebugProbeGITarget.View = std::make_shared<TextureView>(m_DebugProbeGITarget.Texture, viewDescriptor, "DebugProbeGITarget_View");
		}

		const GraphicsBackendRenderTargetDescriptor colorDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = m_DebugProbeGITarget.Texture->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

		GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
		GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

		GraphicsBackend::Current()->BeginRenderPass("RaytracingProbes::DebugDrawProbeGI");
		{
			Profiler::GPUMarker gpuMarker("RaytracingProbes::DebugDrawProbeGI", GPUQueue::RENDER);

			GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());

			BindResources();
			GraphicsBackend::Current()->BindTexture(renderData.GBuffers[1].View->GetBackendTextureView(), 0);
			GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget.View->GetBackendTextureView(), 1);
			GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDebugDataBuffer->GetBackendBuffer(), 1, 0, sizeof(debugData));

			const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
			GraphicsBackend::Current()->UseProgram(m_DebugDrawProbeGIShader->GetProgram(fullscreenMesh));
			GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
		}
		GraphicsBackend::Current()->EndRenderPass();

		TextureViewer::RegisterTexture(m_DebugProbeGITarget.View, "Raytracing/DebugProbeGI");
	}
}

void RaytracingProbes::BindResources() const
{
	if (!m_RaytracingProbesEnabled)
		return;

	GraphicsBackend::Current()->BindTextureSampler(m_ProbeLightAtlas.View->GetBackendTextureView(), m_ProbeLightAtlas.Texture->GetBackendSampler(), GlobalConstants::TextureIndex::PROBE_LIGHT);
	GraphicsBackend::Current()->BindTexture(m_ProbeDepthAtlas.View->GetBackendTextureView(), GlobalConstants::TextureIndex::PROBE_DISTANCES);
	GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDataBuffer->GetBackendBuffer(), GlobalConstants::ConstantBufferIndex::PROBE_DATA, 0, sizeof(ProbesData));
}

void RaytracingProbes::LoadShaders(bool reload)
{
	m_ProbeTraceShader = Resources::LoadShader("core_resources/shaders/raytracing/raytracing_probe_trace", {"_RECEIVE_SHADOWS"}, reload);
	m_ProbeIntegrateShader = Resources::LoadShader("core_resources/shaders/raytracing/raytracing_probe_integrate", {}, reload);
	m_ProbeAddBorderShader = Resources::LoadShader("core_resources/shaders/raytracing/raytracing_probe_add_border", {}, reload);
	m_DebugDrawProbesShader = Resources::LoadShader("core_resources/shaders/raytracing/raytracing_debug_draw_probes", {}, reload);
	m_DebugDrawProbeGIShader = Resources::LoadShader("core_resources/shaders/raytracing/raytracing_debug_draw_probe_gi", {"DEFERRED_LIGHTING", "PROBE_GI"}, reload);
}

void RaytracingProbes::UpdateProbesData() const
{
	ProbesData data;
	data.GridSize = m_ProbesGridSize;
	data.Spacing = m_ProbeSpacing;
	data.ProbesPerRow = RaytracingProbesLocal::k_ProbesPerAtlasRow;
	data.ProbeCount = m_ProbesGridSize.x * m_ProbesGridSize.y * m_ProbesGridSize.z;
	data.UpdateProbeBaseIndex = m_UpdateProbeBaseIndex;
	data.ProbeLightSize = m_ProbeLightSize;
	data.ProbesUpdatePerFrame = m_ProbesPerUpdate;
	data.ProbeLightPaddedSize = m_ProbeLightSize + m_ProbeLightPadding + RaytracingProbesLocal::k_ProbesBorderSize;
	data.InvProbeAtlasSize = Vector2(1.0f / m_ProbeLightAtlas.Texture->GetWidth(), 1.0f / m_ProbeLightAtlas.Texture->GetHeight());

	m_ProbesDataBuffer->SetData(&data, 0, sizeof(data));
}

void RaytracingProbes::UpdateTextureResources(TextureResources& textureResources, uint32_t width, uint32_t height, TextureInternalFormat format, const std::string& name) const
{
	if (textureResources.Texture && textureResources.Texture->GetWidth() == width && textureResources.Texture->GetHeight() == height)
		return;

	GraphicsBackendTextureDescriptor descriptor{};
	descriptor.Linear = true;
	descriptor.ReadWrite = true;
	descriptor.Format = format;
	descriptor.Width = width;
	descriptor.Height = height;

	GraphicsBackendTextureViewDescriptor viewDescriptor{};
	viewDescriptor.Format = format;

	textureResources.Texture = Texture2D::Create(descriptor, name);
	textureResources.View = std::make_shared<TextureView>(textureResources.Texture, viewDescriptor, name + "_View");

	viewDescriptor.ReadWrite = true;
	textureResources.RWView = std::make_shared<TextureView>(textureResources.Texture, viewDescriptor, name + "_RWView");
}
