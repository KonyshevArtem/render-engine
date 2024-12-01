#ifndef RENDER_ENGINE_RENDER_PASS_H
#define RENDER_ENGINE_RENDER_PASS_H

struct Context;

class RenderPass
{
public:
    explicit RenderPass(int priority);
    virtual ~RenderPass() = default;

    virtual void Execute(const Context& ctx) = 0;

    RenderPass(const RenderPass &) = delete;
    RenderPass(RenderPass &&) = delete;

    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

private:
    int m_Priority;
};

#endif //RENDER_ENGINE_RENDER_PASS_H
