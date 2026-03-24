#ifndef RENDER_ENGINE_POST_PROCESS_PASS_H
#define RENDER_ENGINE_POST_PROCESS_PASS_H

#include "render_pass.h"

struct RenderData;
class Shader;
class GraphicsBuffer;
class Texture;

class PostProcessPass : public RenderPass
{
public:
    explicit PostProcessPass(int priority);
    ~PostProcessPass() override = default;

    void Prepare(RenderData& renderData);
    void Execute(const RenderData& renderData) override;

private:
    std::shared_ptr<Shader> m_PostProcessShader;
    std::shared_ptr<GraphicsBuffer> m_PostProcessDataBuffer;
    std::shared_ptr<Texture> m_PostProcessedTarget;
};


#endif //RENDER_ENGINE_POST_PROCESS_PASS_H
