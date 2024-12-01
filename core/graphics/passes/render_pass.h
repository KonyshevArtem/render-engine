#ifndef RENDER_ENGINE_RENDER_PASS_H
#define RENDER_ENGINE_RENDER_PASS_H

#include <memory>

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

    struct Comparer
    {
        bool operator()(const std::shared_ptr<RenderPass>& renderPassA, const std::shared_ptr<RenderPass>& renderPassB) const;
    };

private:
    int m_Priority;
};

#endif //RENDER_ENGINE_RENDER_PASS_H
