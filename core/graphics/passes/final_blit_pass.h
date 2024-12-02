#ifndef RENDER_ENGINE_FINAL_BLIT_PASS_H
#define RENDER_ENGINE_FINAL_BLIT_PASS_H

#include "render_pass.h"
#include <memory>

struct Context;
class Texture2D;

class FinalBlitPass : public RenderPass
{
public:
    explicit FinalBlitPass(int priority);
    ~FinalBlitPass() override = default;

    void Prepare(const std::shared_ptr<Texture2D>& source);
    void Execute(const Context& ctx) override;

    FinalBlitPass(const FinalBlitPass&) = delete;
    FinalBlitPass(FinalBlitPass&&) = delete;

    FinalBlitPass &operator=(const FinalBlitPass&) = delete;
    FinalBlitPass &operator=(FinalBlitPass&&) = delete;

private:
    std::shared_ptr<Texture2D> m_Source;
};


#endif //RENDER_ENGINE_FINAL_BLIT_PASS_H
