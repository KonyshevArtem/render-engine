#ifndef RENDER_ENGINE_GBUFFER_PASS_H
#define RENDER_ENGINE_GBUFFER_PASS_H

#include "render_pass.h"
#include "graphics/render_queue/render_queue.h"
#include "texture/texture.h"

class GBufferPass : public RenderPass
{
public:
	GBufferPass() = default;
	~GBufferPass() override = default;

	void Prepare(RenderData& renderData) override;
	void Execute(const RenderData& renderData) override;

private:
	std::shared_ptr<Texture> m_GBuffers[2];
	std::shared_ptr<Texture> m_CameraDepthTarget;

	RenderQueue m_RenderQueue;
};

#endif