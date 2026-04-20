#include "../common/global_defines.h"
#include "../common/camera_data.h"
#include "../common/lighting.h"
#include "../common/helper_functions.h"
#include "raytracing_common.h"

Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> NormalTexture : register(t1);

cbuffer RaytracedShadowsData : register(b0)
{
    float2 InvTargetSize;
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
    if (depth == 1 || dot(normal, lightDir) <= 0)
        discard;
    
    RayDesc ray;
    ray.Origin = worldPos;
    ray.Direction = lightDir;
    ray.TMin = 0.01;
    ray.TMax = 1000;

    RayQuery<RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH> query;
    query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
    query.Proceed();

    if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
        return 1.0f;
    
    return 0.0f;
}