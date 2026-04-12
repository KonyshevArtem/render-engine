#include "raytracing_scene.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "developer_console/developer_console.h"
#include "types/graphics_backend_blas_descriptor.h"
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
			m_TLAS = GraphicsBackend::Current()->CreateTLAS(m_TLASInstances, "RT_Scene");
	}
	GraphicsBackend::Current()->EndComputePass();
}
