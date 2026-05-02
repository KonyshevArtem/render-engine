#include "draw_call_comparer.h"
#include "material/material.h"
#include "graphics/draw_call_info.h"

bool DrawCallComparer::operator()(const DrawCallInfo& drawCallA, const DrawCallInfo& drawCallB) const
{
    const int renderQueue1 = drawCallA.Material->GetRenderQueue();
    const int renderQueue2 = drawCallB.Material->GetRenderQueue();
    if (renderQueue1 != renderQueue2)
        return renderQueue1 <= renderQueue2;

    if (SortMode == DrawCallSortMode::MATERIAL)
        return drawCallA.Material < drawCallB.Material;

    const float distance1 = Vector3::Dot(drawCallA.AABB.GetCenter(), CameraDirection);
    const float distance2 = Vector3::Dot(drawCallB.AABB.GetCenter(), CameraDirection);
    return SortMode == DrawCallSortMode::FRONT_TO_BACK ? distance1 < distance2 : distance1 > distance2;
}
