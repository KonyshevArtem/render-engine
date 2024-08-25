#ifndef SHADOW_CASTER_SHARED_H
#define SHADOW_CASTER_SHARED_H

#include "../common/graphics_backend_macros.h"
#include "../common/per_draw_data.h"
#include "../common/camera_data.h"

struct Attributes
{
    float3 PositionOS ATTRIBUTE(0);
};

float4 vertexFunction(Attributes attributes, CameraDataStruct cameraData, PerDrawDataStruct perDrawData){
    return cameraData._VPMatrix * perDrawData._ModelMatrix * float4(attributes.PositionOS, 1);
}

half4 fragmentFunction()
{
    return half4(1);
}

#endif