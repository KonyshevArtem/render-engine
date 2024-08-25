#ifndef OUTLINE_BLIT_SHARED_H
#define OUTLINE_BLIT_SHARED_H

#include "../common/graphics_backend_macros.h"

struct PerMaterialDataStruct
{
    float4 _Color;
    float4 _BlitTexture_TexelSize;
};

struct Attributes
{
    float3 PositionOS   ATTRIBUTE(0);
    float2 TexCoord     ATTRIBUTE(2);
};

struct Varyings
{
    DECLARE_CLIP_POS(float4, PositionCS)
    float2 Uv;
};

Varyings vertexFunction(Attributes attributes)
{
    Varyings vars;
    OUTPUT_CLIP_POS(vars.PositionCS, float4(attributes.PositionOS, 1));
    vars.Uv = attributes.TexCoord;
#if SCREEN_UV_UPSIDE_DOWN
    vars.Uv.y = 1 - vars.Uv.y;
#endif
    return vars;
}

half4 fragmentFunction(Varyings vars, PerMaterialDataStruct perMaterialData, TEXTURE2D_HALF_PARAMETER(blitTexture, blitTextureSampler))
{
    half a0 = SAMPLE_TEXTURE(blitTexture, blitTextureSampler, vars.Uv + perMaterialData._BlitTexture_TexelSize.zw * float2(2, 0)).r;
    half a1 = SAMPLE_TEXTURE(blitTexture, blitTextureSampler, vars.Uv + perMaterialData._BlitTexture_TexelSize.zw * float2(-2, 0)).r;
    half a2 = SAMPLE_TEXTURE(blitTexture, blitTextureSampler, vars.Uv + perMaterialData._BlitTexture_TexelSize.zw * float2(0, 2)).r;
    half a3 = SAMPLE_TEXTURE(blitTexture, blitTextureSampler, vars.Uv + perMaterialData._BlitTexture_TexelSize.zw * float2(0, -2)).r;
    half maxAlpha = max(a0, max(a1, max(a2, a3)));
    maxAlpha = maxAlpha > half(0.01) ? 1 : 0;

    half alpha = SAMPLE_TEXTURE(blitTexture, blitTextureSampler, vars.Uv).r > half(0.01) ? 1 : 0;
    half diff = max(maxAlpha - alpha, half(0));
    DISCARD(diff < 0.01)

    return half4(perMaterialData._Color);
}

#endif //OUTLINE_BLIT_SHARED_H
