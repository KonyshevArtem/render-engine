#if RENDER_ENGINE_EDITOR

#ifndef RENDER_ENGINE_GIZMOS_PASS_H
#define RENDER_ENGINE_GIZMOS_PASS_H

struct Context;

class GizmosPass
{
public:
    GizmosPass() = default;
    ~GizmosPass() = default;

    void Execute(Context &_context);

    GizmosPass(const GizmosPass &) = delete;
    GizmosPass(GizmosPass &&)      = delete;

    GizmosPass &operator=(const GizmosPass &) = delete;
    GizmosPass &operator=(GizmosPass &&) = delete;
};

#endif
#endif