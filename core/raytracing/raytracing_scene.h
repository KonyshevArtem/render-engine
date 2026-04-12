#ifndef RENDER_ENGINE_RAYTRACING_SCENE_H
#define RENDER_ENGINE_RAYTRACING_SCENE_H

#include "graphics/render_data.h"
#include "types/graphics_backend_tlas.h"
#include "types/graphics_backend_raytracing_instance_descriptor.h"

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

private:
	bool m_RecreateBLASes;
    int m_BLASesPerFrameLimit;

	std::vector<std::shared_ptr<Mesh>> m_PendingBLASes;
	std::vector<GraphicsBackendRaytracingInstanceDescriptor> m_TLASInstances;

	GraphicsBackendTLAS m_TLAS;
};

#endif // RENDER_ENGINE_RAYTRACING_SCENE_H
