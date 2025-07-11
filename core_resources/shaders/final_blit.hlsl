#include "common/global_defines.h"

#define NONE                                    0
#define REINHARD_SIMPLE                         1
#define REINHARD_LUMINANCE                      2
#define REINHARD_LUMINANCE_WHITE_PRESERVING     3
#define ROMBINDAHOUSE                           4
#define FILMIC                                  5
#define UNCHARTED2                              6
#define ACES                                    7
#define ACES_APPROXIMATE                        8

struct Attributes
{
    float3 positionOS : POSITION;
};

struct Varyings
{
    float4 positionCS : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer PerMaterialData : register(b4)
{
    float _OneOverGamma;
    float _Exposure;
    uint _TonemappingMode;
};

Texture2D _BlitTexture : register(t4);
SamplerState sampler_BlitTexture : register(t4);

half3 GammaCorrection(half3 color, float oneOverGamma)
{
    return pow(color, (half3) oneOverGamma);
}

half GetLuminance(half3 color)
{
    return dot(color, half3(0.2126, 0.7152, 0.0722));
}

half3 ReinhardTonemapping(half3 color, float exposure, float oneOverGamma)
{
    color = color * exposure / (color / exposure + (half3) 1.0h);
    return GammaCorrection(color, oneOverGamma);
}

half3 LumaBasedReinhardToneMapping(half3 color, float oneOverGamma)
{
    float luma = GetLuminance(color);
    float toneMappedLuma = luma / (1.0 + luma);
    color *= toneMappedLuma / luma;
    return GammaCorrection(color, oneOverGamma);
}

half3 WhitePreservingLumaBasedReinhardToneMapping(half3 color, float oneOverGamma)
{
    float white = 2.0;
    float luma = GetLuminance(color);
    float toneMappedLuma = luma * (1.0 + luma / (white * white)) / (1.0 + luma);
    color *= toneMappedLuma / luma;
    return GammaCorrection(color, oneOverGamma);
}

half3 RomBinDaHouseToneMapping(half3 color, float oneOverGamma)
{
    color = exp(-1.0 / (2.72 * color + 0.15));
    return GammaCorrection(color, oneOverGamma);
}

half3 FilmicToneMapping(half3 color)
{
    color = max((half3) 0.0h, color - (half3) 0.004h);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

half3 Uncharted2ToneMapping(half3 color, float exposure, float oneOverGamma)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    return GammaCorrection(color, oneOverGamma);
}

half3 ACESTonemapping(half3 color, float oneOverGamma){
    half3x3 m1 = half3x3(
            0.59719, 0.07600, 0.02840,
            0.35458, 0.90834, 0.13383,
            0.04823, 0.01566, 0.83777
    );
    half3x3 m2 = half3x3(
            1.60475, -0.10208, -0.00327,
            -0.53108, 1.10813, -0.07276,
            -0.07367, -0.00605, 1.07602
    );
    half3 v = mul(color, m1);
    half3 a = v * (v + 0.0245786) - 0.000090537;
    half3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    color = clamp(mul((a / b), m2), 0.0, 1.0);
    return GammaCorrection(color, oneOverGamma);
}

half3 ACESApproximateTonemapping(half3 color, float oneOverGamma)
{
    const half a = 2.51;
    const half b = 0.03;
    const half c = 2.43;
    const half d = 0.59;
    const half e = 0.14;
    color = clamp((color * (a * color + b)) / (color * (c * color + d ) + e), 0.0, 1.0);
    return GammaCorrection(color, oneOverGamma);
}

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

    // https://www.shadertoy.com/view/lslGzl
    // https://www.shadertoy.com/view/XsGfWV
    // https://www.shadertoy.com/view/tdffDl

    switch (_TonemappingMode)
    {
        case NONE:
            color = GammaCorrection(color, _OneOverGamma);
            break;
        case REINHARD_SIMPLE:
            color = ReinhardTonemapping(color, _Exposure, _OneOverGamma);
            break;
        case REINHARD_LUMINANCE:
            color = LumaBasedReinhardToneMapping(color, _OneOverGamma);
            break;
        case REINHARD_LUMINANCE_WHITE_PRESERVING:
            color = WhitePreservingLumaBasedReinhardToneMapping(color, _OneOverGamma);
            break;
        case ROMBINDAHOUSE:
            color = RomBinDaHouseToneMapping(color, _OneOverGamma);
            break;
        case FILMIC:
            color = FilmicToneMapping(color);
            break;
        case UNCHARTED2:
            color = Uncharted2ToneMapping(color, _Exposure, _OneOverGamma);
            break;
        case ACES:
            color = ACESTonemapping(color, _OneOverGamma);
            break;
        case ACES_APPROXIMATE:
            color = ACESApproximateTonemapping(color, _OneOverGamma);
            break;
    }

    return half4(color, 1);
}