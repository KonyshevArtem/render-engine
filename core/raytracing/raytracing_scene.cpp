#include "raytracing_scene.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"
#include "mesh/mesh.h"
#include "renderer/renderer.h"
#include "developer_console/developer_console.h"
#include "types/graphics_backend_blas_descriptor.h"
#include "types/graphics_backend_raytracing_instance_descriptor.h"

RaytracingScene::RaytracingScene() :
	m_RecreateBLASes(false)
{
	DeveloperConsole::AddBoolCommand(L"Raytracing.Debug.RecreateBLASes", &m_RecreateBLASes);
}

void RaytracingScene::Update(RenderData& renderData)
{
	Profiler::Marker cpuMarker("RaytracingScene::Update");

	GraphicsBackend::Current()->BeginComputePass("Update Raytracing Scene");

	Profiler::GPUMarker gpuMarker("RaytracingScene::Update");

	for (const std::shared_ptr<Renderer>& renderer : renderData.Renderers)
	{
		if (!renderer)
			continue;

		std::shared_ptr<DrawableGeometry> geometry = renderer->GetGeometry();
		if (!geometry)
			continue;

		const std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh, DrawableGeometry>(geometry);
		if (!mesh || (mesh->BLAS.IsValid() && !m_RecreateBLASes))
			continue;

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
		GraphicsBackend::Current()->DeleteTLAS(m_TLAS);

	std::vector<GraphicsBackendRaytracingInstanceDescriptor> instanceDescriptors;
	instanceDescriptors.reserve(renderData.Renderers.size());

	for (const std::shared_ptr<Renderer>& renderer : renderData.Renderers)
	{
		if (!renderer)
			continue;

		std::shared_ptr<DrawableGeometry> geometry = renderer->GetGeometry();
		if (!geometry)
			continue;

		const std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh, DrawableGeometry>(geometry);
		if (!mesh || !mesh->BLAS.IsValid())
			continue;

		GraphicsBackendRaytracingInstanceDescriptor descriptor{};
		descriptor.BLAS = mesh->BLAS;
		descriptor.Transform = renderer->GetModelMatrix();

		instanceDescriptors.push_back(descriptor);
	}

	if (!instanceDescriptors.empty())
		m_TLAS = GraphicsBackend::Current()->CreateTLAS(instanceDescriptors, "RT_Scene");

	GraphicsBackend::Current()->EndComputePass();

	m_RecreateBLASes = false;
}
