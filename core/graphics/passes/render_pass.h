#ifndef RENDER_ENGINE_RENDER_PASS_H
#define RENDER_ENGINE_RENDER_PASS_H

#include <memory>

struct RenderData;

class RenderPass
{
public:
    RenderPass() = default;
    virtual ~RenderPass() = default;

    virtual void Prepare(RenderData& renderData) = 0;
    virtual void Execute(const RenderData& renderData) = 0;

    RenderPass(const RenderPass &) = delete;
    RenderPass(RenderPass &&) = delete;

    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass &operator=(RenderPass &&) = delete;
};

#endif //RENDER_ENGINE_RENDER_PASS_H
