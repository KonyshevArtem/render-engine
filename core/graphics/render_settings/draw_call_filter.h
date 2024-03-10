#ifndef RENDER_ENGINE_DRAW_CALL_FILTER_H
#define RENDER_ENGINE_DRAW_CALL_FILTER_H

#include <functional>

struct DrawCallInfo;

struct DrawCallFilter
{
    std::function<bool(const DrawCallInfo &)> Delegate;

    static DrawCallFilter Opaque();
    static DrawCallFilter Transparent();
    static DrawCallFilter All();
    static DrawCallFilter ShadowCasters();

    bool operator()(const DrawCallInfo &drawCall) const;
};

#endif //RENDER_ENGINE_DRAW_CALL_FILTER_H
