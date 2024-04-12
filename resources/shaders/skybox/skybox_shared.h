#ifndef RENDER_ENGINE_SKYBOX_SHARED_H
#define RENDER_ENGINE_SKYBOX_SHARED_H

#include "../common/graphics_backend_macros.h"
#include "../common/per_draw_data.h"
#include "../common/camera_data.h"

struct Attributes
{
    float3 positionOS ATTRIBUTE(0);
};

struct Varyings
{
    DECLARE_CLIP_POS(float4, positionCS)
    float3 CubemapUV;
};

Varyings vertexFunction(Attributes attributes, PerDrawDataStruct perDrawData, CameraDataStruct cameraData)
{
    float4 pos = cameraData._VPMatrix * perDrawData._ModelMatrix * float4(attributes.positionOS, 1);
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    pos = pos.xyww;

    Varyings vars;
    OUTPUT_CLIP_POS(vars.positionCS, pos)
    vars.CubemapUV = attributes.positionOS.xyz;
    return vars;
}

half4_type fragmentFunction(Varyings vars, CUBEMAP_HALF_PARAMETER(skybox, skyboxSampler))
{
    return SAMPLE_TEXTURE_LOD(skybox, skyboxSampler, vars.CubemapUV, 0);
}

#endif //RENDER_ENGINE_SKYBOX_SHARED_H
