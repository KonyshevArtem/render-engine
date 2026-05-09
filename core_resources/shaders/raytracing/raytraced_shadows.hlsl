#include "../common/lighting.h"
#include "../common/helper_functions.h"
#include "raytracing_shadows.h"

Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> NormalTexture : register(t1);

Texture2D<float2> BlueNoise : register(t2);
SamplerState sampler_BlueNoise : register(s2);

cbuffer RaytracedShadowsData : register(b0)
{
    float2 InvTargetSize;
    float ShadowsDistance;
    uint SamplesCount;

    float2 Random;
    float2 Padding0;
};

struct Attributes
{
    float3 positionOS : POSITION;
};

float4 vertexMain(Attributes attributes) : SV_Position
{
    return float4(attributes.positionOS.xyz, 1);
}

float fragmentMain(float4 pixelCoord : SV_Position) : SV_Target
{
    float3 normal = NormalTexture.Load(int3(pixelCoord.xy, 0)).xyz * 2 - 1;
    float depth = DepthTexture.Load(int3(pixelCoord.xy, 0));
    
    float2 clipPos = PixelToClipPosition(pixelCoord.xy, InvTargetSize);
    float3 worldPos = ClipToWorldPosition(float3(clipPos, depth), _InvVPMatrix);
    
    float3 lightDir = normalize(-_DirLightDirectionWS);
    if (depth == 1)
        discard;
    
    float2 noiseUV = pixelCoord.xy * InvTargetSize.yy * 20 + Random;
    return TraceShadowRay(worldPos, normal, lightDir, ShadowsDistance
#ifdef RAYTRACED_SOFT_SHADOWS
        , SamplesCount, BlueNoise, sampler_BlueNoise, noiseUV
#endif
        );
}