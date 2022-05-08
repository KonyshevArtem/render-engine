#include "draw_call_info.h"

static constexpr int TRANSPARENT_RENDER_QUEUE = 3000;

bool DrawCallInfo::Comparer::operator()(const DrawCallInfo &_i1, const DrawCallInfo &_i2)
{
    int renderQueue1 = _i1.Material->GetRenderQueue();
    int renderQueue2 = _i2.Material->GetRenderQueue();
    if (renderQueue1 != renderQueue2)
        return renderQueue1 < renderQueue2;

    float distance1 = (_i1.AABB.GetCenter() - CameraPosition).Length();
    float distance2 = (_i2.AABB.GetCenter() - CameraPosition).Length();
    return Sorting == Sorting::FRONT_TO_BACK ? distance1 < distance2 : distance1 > distance2;
}

DrawCallInfo::Filter DrawCallInfo::Filter::Opaque()
{
    return DrawCallInfo::Filter {[](const DrawCallInfo &_i)
                                 { return _i.Material->GetRenderQueue() < TRANSPARENT_RENDER_QUEUE; }};
}

DrawCallInfo::Filter DrawCallInfo::Filter::Transparent()
{
    return DrawCallInfo::Filter {[](const DrawCallInfo &_i)
                                 { return _i.Material->GetRenderQueue() >= TRANSPARENT_RENDER_QUEUE; }};
}

DrawCallInfo::Filter DrawCallInfo::Filter::All()
{
    return DrawCallInfo::Filter {[](const DrawCallInfo &_i)
                                 { return true; }};
}

bool DrawCallInfo::Filter::operator()(const DrawCallInfo &_info)
{
    return Delegate != nullptr ? Delegate(_info) : false;
}