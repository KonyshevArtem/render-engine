#ifndef RENDER_ENGINE_RAYTRACING_SCENE_H
#define RENDER_ENGINE_RAYTRACING_SCENE_H

#include "graphics/render_data.h"
#include "types/graphics_backend_tlas.h"

class RaytracingScene
{
public:
	RaytracingScene();
	~RaytracingScene() = default;

	void Update(RenderData& renderData);

private:
	bool m_RecreateBLASes;

	GraphicsBackendTLAS m_TLAS;
};

#endif // RENDER_ENGINE_RAYTRACING_SCENE_H
