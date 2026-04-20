#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "global_defines.h"

float2 PixelToClipPosition(uint2 pixelPos, float2 invTargetSize)
{
	float2 clipPos = float2(pixelPos.xy) * invTargetSize * 2 - 1;
#if SCREEN_UV_UPSIDE_DOWN
	clipPos.y = -clipPos.y;
#endif
	return clipPos;
}

float3 ClipToWorldPosition(float3 clipPos, float4x4 invVPMatrix)
{
	float4 worldPos = mul(invVPMatrix, float4(clipPos, 1));
	worldPos /= worldPos.w;
	return worldPos.xyz;
}

#endif