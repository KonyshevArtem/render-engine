#include "../../common/global_defines.h"

Texture2D<float4> srcTexture : register(t0);
RWTexture2D<float4> dstTexture : register(u0);

cbuffer Data : register(b0)
{
    uint2 Size;
    float2 Padding;

    uint4 ColorMask;
}

float4 GetBackgroundColor(uint2 dtid)
{
    uint2 cell = dtid.xy / 30;
    bool isEven = (cell.x ^ cell.y) & 1;
    return isEven ? float4(1.0, 1.0, 1.0, 1.0) : float4(0.6, 0.6, 0.6, 1.0);
}

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (any(dtid.xy >= Size))
        return;

    uint2 srcPixel = dtid.xy;
#if !SCREEN_UV_UPSIDE_DOWN
    srcPixel.y = Size.y - srcPixel.y - 1;
#endif

    float4 color = srcTexture.Load(int3(srcPixel.xy, 0));
    if (all(ColorMask == uint4(0, 0, 0, 1)))
        color = float4(color.w, color.w, color.w, 1);
    else
    {
        color.xyz *= ColorMask.xyz;
        color.w = ColorMask.w ? color.w : 1;
    }

    float4 backgroundColor = GetBackgroundColor(dtid.xy);

    color.xyz = lerp(backgroundColor.xyz, color.xyz, color.w);
    color.w = 1;

    dstTexture[dtid.xy] = color;
}