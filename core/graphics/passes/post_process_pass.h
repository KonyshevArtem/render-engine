#ifndef RENDER_ENGINE_POST_PROCESS_PASS_H
#define RENDER_ENGINE_POST_PROCESS_PASS_H

#include "render_pass.h"
#include "texture/texture_resources.h"

struct RenderData;
class Shader;
class GraphicsBuffer;

class PostProcessPass : public RenderPass
{
public:
    PostProcessPass();
    ~PostProcessPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

private:
    std::shared_ptr<Shader> m_PostProcessShader;
    std::shared_ptr<GraphicsBuffer> m_PostProcessDataBuffer;
    TextureResources m_PostProcessedTarget;
};


#endif //RENDER_ENGINE_POST_PROCESS_PASS_H
