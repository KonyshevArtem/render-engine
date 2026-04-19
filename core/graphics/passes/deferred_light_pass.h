#ifndef RENDER_ENGINE_DEFERRED_LIGHT_PASS_H
#define RENDER_ENGINE_DEFERRED_LIGHT_PASS_H

#include "render_pass.h"
#include "graphics_buffer/graphics_buffer.h"
#include "shader/shader.h"
#include "texture/texture.h"

class DeferredLightPass : public RenderPass
{
public:
	DeferredLightPass();
	~DeferredLightPass() override = default;

	void Prepare(RenderData& renderData) override;
	void Execute(const RenderData& renderData) override;

private:
	std::shared_ptr<Shader> m_LightShader;
	std::shared_ptr<Texture> m_CameraColorTarget;
	std::shared_ptr<GraphicsBuffer> m_LightingDataBuffer;
};

#endif