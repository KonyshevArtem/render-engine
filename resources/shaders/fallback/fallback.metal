#include "fallback_shared.h"

Varyings vertex vertexMain(Attributes input [[stage_in]],
                           constant PerDrawDataStruct & PerDrawData [[buffer(1)]],
                           constant CameraDataStruct & CameraData [[buffer(2)]])
{
    return vertexFunction(input, PerDrawData, CameraData);
}

half4 fragment fragmentMain(Varyings varyings [[stage_in]])
{
    return fragmentFunction();
}
