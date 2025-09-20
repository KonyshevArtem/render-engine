#include "draw_call_comparer.h"
#include "renderer/renderer.h"
#include "material/material.h"
#include "graphics/draw_call_info.h"

bool DrawCallComparer::operator()(const DrawCallInfo &drawCallA, const DrawCallInfo &drawCallB)
{
    int renderQueue1 = drawCallA.Material->GetRenderQueue();
    int renderQueue2 = drawCallB.Material->GetRenderQueue();
    if (renderQueue1 != renderQueue2)
        return renderQueue1 <= renderQueue2;

    float distance1 = Vector3::Dot(drawCallA.AABB.GetCenter(), CameraDirection);
    float distance2 = Vector3::Dot(drawCallB.AABB.GetCenter(), CameraDirection);
    return SortMode == DrawCallSortMode::FRONT_TO_BACK ? distance1 < distance2 : distance1 > distance2;
}
