#ifndef RENDER_ENGINE_FORWARD_RENDER_PASS_H
#define RENDER_ENGINE_FORWARD_RENDER_PASS_H

#include "render_pass.h"
#include "types/graphics_backend_fence.h"
#include "graphics/render_queue/render_queue.h"

class DrawRenderersPass;
class SkyboxPass;
class Renderer;
class Texture;

class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass();
    ~ForwardRenderPass() override;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;
    const GraphicsBackendFence& GetEndFence() const;

private:
	RenderQueue m_RenderQueue;
    GraphicsBackendFence m_EndFence;
};

#endif //RENDER_ENGINE_FORWARD_RENDER_PASS_H
