#include "common/global_defines.h"

cbuffer PerMaterialData
{
    float4 _Color;
    float4 _BlitTexture_TexelSize;
};

struct Attributes
{
    float3 PositionOS   : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct Varyings
{
    float4 PositionCS     : SV_Position;
    float2 Uv             : TEXCOORD0;
};

Texture2D _BlitTexture;
SamplerState sampler_BlitTexture;

Varyings vertexMain(Attributes attributes)
{
    Varyings vars;
    vars.PositionCS = float4(attributes.PositionOS, 1);
    vars.Uv = attributes.TexCoord;
#if SCREEN_UV_UPSIDE_DOWN
    vars.Uv.y = 1 - vars.Uv.y;
#endif
    return vars;
}

half4 fragmentMain(Varyings vars) : SV_Target
{
    half a0 = _BlitTexture.Sample(sampler_BlitTexture, vars.Uv + _BlitTexture_TexelSize.zw * float2(2, 0)).r;
    half a1 = _BlitTexture.Sample(sampler_BlitTexture, vars.Uv + _BlitTexture_TexelSize.zw * float2(-2, 0)).r;
    half a2 = _BlitTexture.Sample(sampler_BlitTexture, vars.Uv + _BlitTexture_TexelSize.zw * float2(0, 2)).r;
    half a3 = _BlitTexture.Sample(sampler_BlitTexture, vars.Uv + _BlitTexture_TexelSize.zw * float2(0, -2)).r;
    half maxAlpha = max(a0, max(a1, max(a2, a3)));
    maxAlpha = maxAlpha > half(0.01) ? 1 : 0;

    half alpha = _BlitTexture.Sample(sampler_BlitTexture, vars.Uv).r > 0.01h ? 1 : 0;
    half diff = max(maxAlpha - alpha, 0.0h);
    if (diff < 0.01h)
        discard;

    return half4(_Color);
}
