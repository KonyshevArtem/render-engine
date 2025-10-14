#include "common/per_draw_data.h"
#include "common/camera_data.h"

struct Attributes
{
    float3 PositionOS : POSITION;
};

cbuffer ShadowCasterPassData : register(b0)
{
    float4 _LightPosOrDir;

    float3 _Padding0;
    float _DepthBias;
}

float4 vertexMain(Attributes attributes) : SV_Position
{
    float4 posWS = mul(_ModelMatrix, float4(attributes.PositionOS, 1));

    float3 lightDir = _LightPosOrDir.xyz;
    if (_LightPosOrDir.w == 1)
        lightDir = normalize(posWS.xyz - lightDir);
    posWS.xyz += lightDir * _DepthBias;

    return mul(_VPMatrix, posWS);
}

half4 fragmentMain() : SV_Target
{
    return (half4) 0.0h;
}