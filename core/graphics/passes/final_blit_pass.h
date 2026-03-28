#ifndef RENDER_ENGINE_FINAL_BLIT_PASS_H
#define RENDER_ENGINE_FINAL_BLIT_PASS_H

#include "render_pass.h"

struct RenderData;
class Shader;

class FinalBlitPass : public RenderPass
{
public:
    FinalBlitPass();
    ~FinalBlitPass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

private:
    std::shared_ptr<Shader> m_BlitShader;
};


#endif //RENDER_ENGINE_FINAL_BLIT_PASS_H
