#ifndef RENDER_ENGINE_FINAL_BLIT_PASS_H
#define RENDER_ENGINE_FINAL_BLIT_PASS_H

#include <memory>

struct Context;
class Texture2D;

class FinalBlitPass
{
public:
    FinalBlitPass() = default;
    ~FinalBlitPass() = default;

    void Execute(Context &context, const std::shared_ptr<Texture2D> &source);

    FinalBlitPass(const FinalBlitPass &) = delete;
    FinalBlitPass(FinalBlitPass &&)      = delete;

    FinalBlitPass &operator=(const FinalBlitPass &) = delete;
    FinalBlitPass &operator=(FinalBlitPass &&) = delete;
};


#endif //RENDER_ENGINE_FINAL_BLIT_PASS_H
