#include "../common/global_defines.h"
#include "../common/shadows.h"

struct Attributes
{
    float3 vertPositionOS   : POSITION;
    float2 texCoord         : TEXCOORD;
};

struct Varyings
{
    float4 PositionCS       : SV_POSITION;
    float2 UV               : TEXCOORD2;
};

Texture2D<float> _Depth : register(t0);
SamplerState sampler_Depth : register(s0);

cbuffer DebugData : register(b0)
{
    float4x4 _InvCameraVP;
}

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;

    vars.PositionCS = float4(attributes.vertPositionOS.xy, 0.5, 1);
    vars.UV = attributes.texCoord;

    return vars;
}


float4 fragmentMain(Varyings vars) : SV_Target
{
    float2 depthUV = vars.UV;
#if SCREEN_UV_UPSIDE_DOWN
    depthUV.y = 1 - depthUV.y;
#endif

    float depth = _Depth.Sample(sampler_Depth, depthUV);
    float4 posWS = mul(_InvCameraVP, float4(vars.UV.xy * 2 - 1, depth * 2 - 1, 1));
    posWS /= posWS.w;

    for (int i = 0; i < SHADOW_CASCADE_COUNT; ++i)
    {
        float3 shadowCoord = mul(_DirLightShadow[i].LightViewProjMatrix, float4(posWS.xyz, 1)).xyz;
        if (isFragVisibleXY(shadowCoord.xy))
        {
            float r = i == 2 || i == 3 ? 1 : 0;
            float g = i == 1 || i == 2 ? 1 : 0;
            float b = i == 0 ? 1 : 0;
            return float4(r, g, b, 0.25);
        }
    }

    return float4(0, 0, 0, 0);
}