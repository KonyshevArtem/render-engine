#include "raytracing_scene.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "developer_console/developer_console.h"
#include "types/graphics_backend_blas_descriptor.h"
#include "types/graphics_backend_buffer_descriptor.h"
#include "types/graphics_backend_raytracing_instance_descriptor.h"

RaytracingScene::RaytracingScene() :
	m_RecreateBLASes(false),
	m_BLASesPerFrameLimit(10)
{
	DeveloperConsole::AddBoolCommand(L"Raytracing.Debug.RecreateBLASes", &m_RecreateBLASes);
	DeveloperConsole::AddIntCommand(L"Raytracing.Debug.BLASPerFrameLimit", &m_BLASesPerFrameLimit);
}


void RaytracingScene::Prepare(RenderData& renderData)
{
	Profiler::Marker _("RaytracingScene::Prepare");

	m_PendingBLASes.clear();
	m_TLASInstances.clear();
	m_PerInstanceData.clear();

    for (const std::shared_ptr<Renderer>& renderer : renderData.Renderers)
	{
		if (!renderer)
			continue;

		std::shared_ptr<DrawableGeometry> geometry = renderer->GetGeometry();
		if (!geometry)
			continue;

		const std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh, DrawableGeometry>(geometry);
		if (!mesh)
			continue;

        if (!mesh->BLAS.IsValid() || m_RecreateBLASes)
		{
			if (mesh->BLAS.IsValid())
			{
				GraphicsBackend::Current()->DeleteBLAS(mesh->BLAS);
				mesh->BLAS = GraphicsBackendBLAS{};
			}

            if (m_PendingBLASes.size() < m_BLASesPerFrameLimit)
                m_PendingBLASes.push_back(mesh);
		}
		else
		{
			GraphicsBackendRaytracingInstanceDescriptor descriptor{};
			descriptor.BLAS = mesh->BLAS;
			descriptor.Transform = renderer->GetModelMatrix();

			m_TLASInstances.push_back(descriptor);

			PerInstanceData data{};
			data.VerticesBufferIndex = mesh->GetVertexBufferView().BindlessIndex;
			data.IndicesBufferIndex = mesh->GetIndexBufferView().BindlessIndex;

			GraphicsBackendVertexAttributeDescriptor vertexAttributeDescriptor;
			if (mesh->GetVertexAttributes().TryGetAttribute(VertexAttributeSemantic::POSITION, vertexAttributeDescriptor))
				data.VertexStride = vertexAttributeDescriptor.Stride;

			m_PerInstanceData.push_back(data);
		}
	}

	m_RecreateBLASes = false;
}

void RaytracingScene::Update(RenderData& renderData)
{
	Profiler::Marker cpuMarker("RaytracingScene::Update");

	GraphicsBackend::Current()->BeginComputePass("Update Raytracing Scene");
	{
		Profiler::GPUMarker gpuMarker("RaytracingScene::Update");

        for (const std::shared_ptr<Mesh>& mesh : m_PendingBLASes)
        {
			GraphicsBackendBLASDescriptor descriptor{};
			if (!mesh->GetVertexAttributes().TryGetAttribute(VertexAttributeSemantic::POSITION, descriptor.VertexAttributeDescriptor))
				continue;

			descriptor.Geometry = mesh->GetGraphicsBackendGeometry();
			descriptor.VertexCount = mesh->GetVertexCount();
			descriptor.IndexCount = mesh->GetIndexCount();
			descriptor.IndicesDataType = mesh->GetIndicesDataType();

            mesh->BLAS = GraphicsBackend::Current()->CreateBLAS(descriptor, mesh->GetName());
		}

		if (m_TLAS.IsValid())
		{
			GraphicsBackend::Current()->DeleteTLAS(m_TLAS);
			m_TLAS = GraphicsBackendTLAS{};
		}

		if (!m_TLASInstances.empty())
		{
			m_TLAS = GraphicsBackend::Current()->CreateTLAS(m_TLASInstances, "RT_Scene");

			const uint32_t perInstanceDataRequiredSize = sizeof(PerInstanceData) * m_PerInstanceData.size();
			if (!m_PerInstanceDataBuffer || m_PerInstanceDataBuffer->GetSize() < perInstanceDataRequiredSize)
			{
				GraphicsBackendBufferDescriptor bufferDescriptor{};
				bufferDescriptor.Size = perInstanceDataRequiredSize;
				bufferDescriptor.AllowCPUWrites = true;
				m_PerInstanceDataBuffer = std::make_shared<GraphicsBuffer>(bufferDescriptor, "RT_Scene_PerInstanceData", nullptr);

				GraphicsBackendBufferViewDescriptor bufferViewDescriptor = GraphicsBackendBufferViewDescriptor::Structured(m_PerInstanceData.size(), sizeof(PerInstanceData), 0, false);
				m_PerInstanceDataBufferView = std::make_shared<GraphicsBufferView>(m_PerInstanceDataBuffer, bufferViewDescriptor, "RT_Scene_PerInstanceDataView");
			}

			GraphicsBackend::Current()->SetBufferData(m_PerInstanceDataBuffer->GetBackendBuffer(), 0, perInstanceDataRequiredSize, m_PerInstanceData.data());
		}
	}
	GraphicsBackend::Current()->EndComputePass();
}
