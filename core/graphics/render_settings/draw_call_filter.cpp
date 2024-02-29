#include "draw_call_filter.h"
#include "graphics/draw_call_info.h"
#include "global_constants.h"

DrawCallFilter DrawCallFilter::Opaque()
{
    return DrawCallFilter{[](const DrawCallInfo &drawCall) { return drawCall.Material->GetRenderQueue() < GlobalConstants::TransparentRenderQueue; }};
}

DrawCallFilter DrawCallFilter::Transparent()
{
    return DrawCallFilter{[](const DrawCallInfo &drawCall) { return drawCall.Material->GetRenderQueue() >= GlobalConstants::TransparentRenderQueue; }};
}

DrawCallFilter DrawCallFilter::All()
{
    return DrawCallFilter{[](const DrawCallInfo &drawCall) { return true; }};
}

DrawCallFilter DrawCallFilter::ShadowCasters()
{
    return DrawCallFilter{[](const DrawCallInfo &drawCall) { return drawCall.CastShadows; }};
}

bool DrawCallFilter::operator()(const DrawCallInfo &_info) const
{
    return Delegate != nullptr && Delegate(_info);
}