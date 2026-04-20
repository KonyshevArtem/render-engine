#include "raytracing_pass.h"
#include "shader/shader.h"
#include "developer_console/developer_console.h"
#include "graphics_backend_api.h"
#include "raytracing_scene.h"
#include "editor/profiler/profiler.h"
#include "enums/resource_state.h"
#include "graphics/render_data.h"
#include "texture_2d/texture_2d.h"
#include "graphics_buffer/graphics_buffer.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "mesh/mesh.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "global_constants.h"

RaytracingPass::RaytracingPass(const std::shared_ptr<RaytracingScene>& rtScene) :
	m_PrimaryRaysDebugEnabled(false),
	m_RaytracingScene(rtScene)
{
	m_PrimaryRaysDebugShader = Shader::Load("core_resources/shaders/raytracing/primary_rays_debug", {"_RECEIVE_SHADOWS"});

	DeveloperConsole::AddBoolCommand(L"Raytracing.Debug.PrimaryRays", &m_PrimaryRaysDebugEnabled);
}

void RaytracingPass::Prepare(RenderData& renderData)
{
}

void RaytracingPass::Execute(const RenderData& renderData)
{
}

void RaytracingPass::ExecutePrimaryRaysDebug(const RenderData& renderData)
{
	if (!m_PrimaryRaysDebugEnabled || !m_RaytracingScene->GetTLAS().IsValid())
		return;

	Profiler::Marker _("RaytracingPass::ExecutePrimaryRaysDebug");

	GraphicsBackendRenderTargetDescriptor colorTargetDescriptor{};
	colorTargetDescriptor.Texture = renderData.CameraColorTarget->GetBackendTexture();
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

		constants.TargetSize = Vector2UI(renderData.CameraColorTarget->GetWidth(), renderData.CameraColorTarget->GetHeight());

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

		GraphicsBackend::Current()->BindTLAS(m_RaytracingScene->GetTLAS(), GlobalConstants::RTSceneIndex);
		GraphicsBackend::Current()->BindBuffer(m_RaytracingScene->GetPerInstanceDataBufferView()->GetBackendBufferView(), GlobalConstants::RTPerInstanceDataIndex);
		GraphicsBackend::Current()->BindConstantBuffer(m_PrimaryRaysDebugDataBuffer->GetBackendBuffer(), 0, 0, sizeof(constants));

		const std::shared_ptr<Mesh> fullscreenMesh = Mesh::GetFullscreenMesh();
		GraphicsBackend::Current()->UseProgram(m_PrimaryRaysDebugShader->GetProgram(fullscreenMesh));
		GraphicsBackend::Current()->DrawElements(fullscreenMesh->GetGraphicsBackendGeometry(), fullscreenMesh->GetPrimitiveType(), fullscreenMesh->GetElementsCount(), fullscreenMesh->GetIndicesDataType());

	}
	GraphicsBackend::Current()->EndRenderPass();
}
