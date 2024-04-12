#include "fallback_shared.h"

float4 vertex vertexMain(Attributes input [[stage_in]],
                           constant PerDrawDataStruct & PerDrawData [[buffer(1)]],
                           constant CameraDataStruct & CameraData [[buffer(2)]])
{
    return vertexFunction(input, PerDrawData, CameraData);
}

half4 fragment fragmentMain()
{
    return fragmentFunction();
}
