#ifndef RENDER_ENGINE_GIZMOS_PASS_H
#define RENDER_ENGINE_GIZMOS_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"

struct Context;

class GizmosPass : public RenderPass
{
public:
    explicit GizmosPass(int priority);
    ~GizmosPass() override = default;

    void Execute(const Context& ctx) override;

    GizmosPass(const GizmosPass&) = delete;
    GizmosPass(GizmosPass&&) = delete;

    GizmosPass &operator=(const GizmosPass&) = delete;
    GizmosPass &operator=(GizmosPass&&) = delete;
};

#endif
#endif