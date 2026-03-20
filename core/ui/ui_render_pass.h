#ifndef RENDER_ENGINE_UI_RENDER_PASS_H
#define RENDER_ENGINE_UI_RENDER_PASS_H

#include "graphics/passes/render_pass.h"
#include <vector>

class Shader;
class RingBuffer;
class UIElement;

class UIRenderPass : public RenderPass
{
public:
    explicit UIRenderPass(int priority);
    ~UIRenderPass() override = default;

    void Prepare(const RenderData& renderData);
    void Execute(const RenderData& renderData) override;

private:
    std::shared_ptr<Shader> m_ImageShader;
    std::shared_ptr<Shader> m_TextShader;
    std::shared_ptr<Shader> m_MaskStencilShader;
    std::shared_ptr<RingBuffer> m_UIDataBuffer;
};

#endif //RENDER_ENGINE_UI_RENDER_PASS_H
