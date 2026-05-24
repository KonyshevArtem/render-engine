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
	m_FileWatcher.AddFile("core_resources/shaders/common/lighting.h");

	LoadShaders(false);
}

void RaytracingProbes::Prepare(RenderData& renderData)
{
	if (!m_RaytracingProbesEnabled)
	{
		Shader::RemoveGlobalDefine("PROBE_GI");
		return;
	}

	Shader::AddGlobalDefine("PROBE_GI");

	Profiler::Marker _("RaytracingProbes::Prepare");

	if (m_FileWatcher.FilesChanged())
		LoadShaders(true);

	const uint32_t probesCount = m_ProbesGridSize.x * m_ProbesGridSize.y * m_ProbesGridSize.z;
	const uint32_t paddedLightSize = m_ProbeLightSize + m_ProbeLightPadding + RaytracingProbesLocal::k_ProbesBorderSize;

	GraphicsBackendTextureDescriptor descriptor{};
	descriptor.Format = TextureInternalFormat::RGBA16F;
	descriptor.Linear = true;
	descriptor.ReadWrite = true;

	descriptor.Width = RaytracingProbesLocal::k_ProbesPerAtlasRow * paddedLightSize;
	descriptor.Height = std::max<uint32_t>(probesCount / RaytracingProbesLocal::k_ProbesPerAtlasRow, 1) * paddedLightSize;
	if (!m_ProbeLightAtlas || m_ProbeLightAtlas->GetWidth() != descriptor.Width || m_ProbeLightAtlas->GetHeight() != descriptor.Height)
		m_ProbeLightAtlas = Texture2D::Create(descriptor, "ProbeLightAtlas");

	descriptor.Width = std::min<uint32_t>(m_ProbesPerUpdate, RaytracingProbesLocal::k_ProbesPerAtlasRow) * paddedLightSize;
	descriptor.Height = std::max<uint32_t>(m_ProbesPerUpdate / RaytracingProbesLocal::k_ProbesPerAtlasRow, 1) * paddedLightSize;
	if (!m_ProbeTempAtlas || m_ProbeTempAtlas->GetWidth() != descriptor.Width || m_ProbeTempAtlas->GetHeight() != descriptor.Height)
		m_ProbeTempAtlas = Texture2D::Create(descriptor, "ProbeTempAtlas");
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

		// Trace
		GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDataBuffer->GetBackendBuffer(), 0, 0, sizeof(ProbesData));
		GraphicsBackend::Current()->BindRWTexture(m_ProbeTempAtlas->GetBackendTexture(), 0);
		GraphicsBackend::Current()->UseProgram(m_ProbeTraceShader->GetProgram());
		GraphicsBackend::Current()->Dispatch(m_ProbeLightSize * m_ProbesPerUpdate, m_ProbeLightSize, 1);

		// Integrate
		GraphicsBackend::Current()->BindTexture(m_ProbeTempAtlas->GetBackendTexture(), 0);
		GraphicsBackend::Current()->BindRWTexture(m_ProbeLightAtlas->GetBackendTexture(), 0);
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

	TextureViewer::RegisterTexture(m_ProbeTempAtlas, "Raytracing/ProbeTempAtlas");
	TextureViewer::RegisterTexture(m_ProbeLightAtlas, "Raytracing/ProbeLightAtlas");
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
	debugData.InvTargetSize = Vector2(1.0f / renderData.CameraDepthTarget->GetWidth(), 1.0f / renderData.CameraDepthTarget->GetHeight());
	m_ProbesDebugDataBuffer->SetData(&debugData, 0, sizeof(debugData));

	UpdateProbesData();

	if (m_DebugDrawProbes && m_DebugDrawProbesShader && m_DebugDrawProbesShader->IsValid())
	{
		Profiler::Marker _("RaytracingProbes::DebugDrawProbes");

		const GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = renderData.CameraColorTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };
		const GraphicsBackendRenderTargetDescriptor depthTargetDescriptor{ .Attachment = FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT, .Texture = renderData.CameraDepthTarget->GetBackendTexture(), .LoadAction = LoadAction::LOAD };

		GraphicsBackend::Current()->AttachRenderTarget(colorTargetDescriptor);
		GraphicsBackend::Current()->AttachRenderTarget(depthTargetDescriptor);

		GraphicsBackend::Current()->BeginRenderPass("RaytracingProbes::DebugDrawProbes");
		{
			Profiler::GPUMarker gpuMarker("RaytracingProbes::DebugDrawProbes");

			BindResources();
			GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDebugDataBuffer->GetBackendBuffer(), 1, 0, sizeof(debugData));
			GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget->GetBackendTexture(), 1);

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
		if (m_DebugProbeGITarget == nullptr || m_DebugProbeGITarget->GetWidth() != width || m_DebugProbeGITarget->GetHeight() != height)
		{
			GraphicsBackendTextureDescriptor descriptor;
			descriptor.Width = width;
			descriptor.Height = height;
			descriptor.Linear = true;
			descriptor.RenderTarget = true;

			descriptor.Format = TextureInternalFormat::RGBA16F;
			m_DebugProbeGITarget = Texture2D::Create(descriptor, "DebugProbeGITarget");
		}

		const GraphicsBackendRenderTargetDescriptor colorDescriptor{ .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .Texture = m_DebugProbeGITarget->GetBackendTexture(), .LoadAction = LoadAction::CLEAR };

		GraphicsBackend::Current()->AttachRenderTarget(colorDescriptor);
		GraphicsBackend::Current()->AttachRenderTarget(GraphicsBackendRenderTargetDescriptor::EmptyDepth());

		GraphicsBackend::Current()->BeginRenderPass("RaytracingProbes::DebugDrawProbeGI");
		{
			Profiler::GPUMarker gpuMarker("RaytracingProbes::DebugDrawProbeGI", GPUQueue::RENDER);

			GraphicsBackend::Current()->SetDepthState(GraphicsBackendDepthDescriptor::Disabled());

			BindResources();
			GraphicsBackend::Current()->BindTexture(renderData.GBuffers[1]->GetBackendTexture(), 0);
			GraphicsBackend::Current()->BindTexture(renderData.CameraDepthTarget->GetBackendTexture(), 1);
			GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDebugDataBuffer->GetBackendBuffer(), 1, 0, sizeof(debugData));

			const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
			GraphicsBackend::Current()->UseProgram(m_DebugDrawProbeGIShader->GetProgram(fullscreenMesh));
			GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());
		}
		GraphicsBackend::Current()->EndRenderPass();

		TextureViewer::RegisterTexture(m_DebugProbeGITarget, "Raytracing/DebugProbeGI");
	}
}

void RaytracingProbes::BindResources() const
{
	if (!m_RaytracingProbesEnabled)
		return;

	GraphicsBackend::Current()->BindTextureSampler(m_ProbeLightAtlas->GetBackendTexture(), m_ProbeLightAtlas->GetBackendSampler(), 3);
	GraphicsBackend::Current()->BindConstantBuffer(m_ProbesDataBuffer->GetBackendBuffer(), 0, 0, sizeof(ProbesData));
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
	data.InvProbeAtlasSize = Vector2(1.0f / m_ProbeLightAtlas->GetWidth(), 1.0f / m_ProbeLightAtlas->GetHeight());

	m_ProbesDataBuffer->SetData(&data, 0, sizeof(data));
}
