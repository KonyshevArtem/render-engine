#ifndef RENDER_ENGINE_FALLBACK_SHARED_H
#define RENDER_ENGINE_FALLBACK_SHARED_H

#include "../common/graphics_backend_macros.h"
#include "../common/per_draw_data.h"
#include "../common/camera_data.h"

struct Attributes
{
    float3 positionOS ATTRIBUTE(0);
};

float4 vertexFunction(Attributes attributes, PerDrawDataStruct perDrawData, CameraDataStruct cameraData)
{
    return cameraData._VPMatrix * perDrawData._ModelMatrix * float4(attributes.positionOS, 1.0);
}

half4_type fragmentFunction()
{
    return half4(1, 0, 1, 1);
}

#endif //RENDER_ENGINE_FALLBACK_SHARED_H
