#include "common/camera_data.h"
#include "common/per_draw_data.h"
#include "common/lighting.h"

struct Attributes
{
    float3 positionOS : POSITION;
};

struct Varyings
{
    float4 positionCS : SV_POSITION;
    float3 CubemapUV : TEXCOORD0;
};

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;
    vars.positionCS = mul(_VPMatrix, mul(_ModelMatrix, float4(attributes.positionOS, 1.0)));
    // make z equals w so it would be 1 after perspective divide to render skybox behind all geometry
    vars.positionCS = vars.positionCS.xyww;

    vars.CubemapUV = attributes.positionOS.xyz;
    return vars;
}

half4 fragmentMain(Varyings vars) : SV_Target
{
    return _ReflectionCube.SampleLevel(sampler_ReflectionCube, vars.CubemapUV, 0);
}
