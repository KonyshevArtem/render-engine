#ifndef RENDER_ENGINE_RAYTRACING_SCENE_H
#define RENDER_ENGINE_RAYTRACING_SCENE_H

#include "graphics/render_data.h"
#include "types/graphics_backend_tlas.h"
#include "types/graphics_backend_raytracing_instance_descriptor.h"
#include "graphics_buffer/graphics_buffer.h"
#include "graphics_buffer/graphics_buffer_view.h"

class Mesh;

class RaytracingScene
{
public:
	RaytracingScene();
	~RaytracingScene() = default;

	void Prepare(RenderData& renderData);
	void Update(RenderData& renderData);

	const GraphicsBackendTLAS& GetTLAS() const
	{
		return m_TLAS;
	}

	const std::shared_ptr<GraphicsBufferView> GetPerInstanceDataBufferView() const
	{
		return m_PerInstanceDataBufferView;
	}

private:
	struct PerInstanceData
	{
		uint32_t VerticesBufferIndex;
		uint32_t IndicesBufferIndex;
		uint32_t VertexStride;
		float Padding0;
	};

	bool m_RecreateBLASes;
    int m_BLASesPerFrameLimit;

	std::vector<std::shared_ptr<Mesh>> m_PendingBLASes;
	std::vector<GraphicsBackendRaytracingInstanceDescriptor> m_TLASInstances;

	std::vector<PerInstanceData> m_PerInstanceData;
	std::shared_ptr<GraphicsBuffer> m_PerInstanceDataBuffer;
	std::shared_ptr<GraphicsBufferView> m_PerInstanceDataBufferView;

	GraphicsBackendTLAS m_TLAS;
};

#endif // RENDER_ENGINE_RAYTRACING_SCENE_H
