#ifndef RENDER_ENGINE_FORWARD_RENDER_PASS_H
#define RENDER_ENGINE_FORWARD_RENDER_PASS_H

#include "render_pass.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "matrix4x4/matrix4x4.h"

#include <memory>
#include <vector>

class DrawRenderersPass;
class SkyboxPass;
class Renderer;
class Texture;
struct Vector3;

class ForwardRenderPass : public RenderPass
{
public:
    explicit ForwardRenderPass(int priority);
    ~ForwardRenderPass() override;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;
    const GraphicsBackendFence& GetEndFence() const;

private:
    std::shared_ptr<DrawRenderersPass> m_OpaquePass;
    std::shared_ptr<DrawRenderersPass> m_TransparentPass;
    std::shared_ptr<SkyboxPass> m_SkyboxPass;

    std::shared_ptr<Texture> m_CameraColorTarget;
    std::shared_ptr<Texture> m_CameraDepthTarget;
    
    GraphicsBackendFence m_EndFence;
};

#endif //RENDER_ENGINE_FORWARD_RENDER_PASS_H
