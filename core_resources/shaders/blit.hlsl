#include "common/global_defines.h"

struct Attributes
{
    float3 positionOS : POSITION;
};

struct Varyings
{
    float4 positionCS : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D _BlitTexture : register(t0);
SamplerState sampler_BlitTexture : register(t0);

Varyings vertexMain(Attributes attributes, uint vid : SV_VertexID)
{
    Varyings vars;
    vars.positionCS = float4(attributes.positionOS.xyz, 1);

    if (vid == 0)
        vars.uv = float2(0, 0);
    else if (vid == 1)
        vars.uv = float2(1, 0);
    else if (vid == 2)
        vars.uv = float2(1, 1);
    else
        vars.uv = float2(0, 1);

#if SCREEN_UV_UPSIDE_DOWN
    vars.uv.y = 1 - vars.uv.y;
#endif

    return vars;
}

half4 fragmentMain(Varyings varyings) : SV_Target
{
    half3 color = _BlitTexture.Sample(sampler_BlitTexture, varyings.uv).rgb;
    return half4(color, 1);
}