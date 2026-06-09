#ifndef RENDER_ENGINE_GBUFFER_PASS_H
#define RENDER_ENGINE_GBUFFER_PASS_H

#include "render_pass.h"
#include "graphics/render_queue/render_queue.h"
#include "texture/texture_resources.h"

class GBufferPass : public RenderPass
{
public:
	GBufferPass() = default;
	~GBufferPass() override = default;

	void Prepare(RenderData& renderData) override;
	void Execute(const RenderData& renderData) override;

private:
	TextureResources m_GBuffers[2];
	TextureResources m_CameraDepthTarget;

	RenderQueue m_RenderQueue;
};

#endif