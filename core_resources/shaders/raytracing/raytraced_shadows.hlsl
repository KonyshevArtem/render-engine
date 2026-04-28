#include "../common/global_defines.h"
#include "../common/camera_data.h"
#include "../common/lighting.h"
#include "../common/helper_functions.h"
#include "raytracing_common.h"

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

struct ConeFrame
{
    float3 Direction;
    float3 Right;
    float3 Forward;
};

ConeFrame GetConeFrame(float3 dir)
{
    ConeFrame frame;
    float3 up = abs(dir.y) < 0.999 ? float3(0, 1, 0) : float3(1, 0, 0);
    frame.Direction = dir;
    frame.Right = normalize(cross(up, dir));
    frame.Forward = cross(dir, frame.Right);
    return frame;
}

float3 ConePerturb(ConeFrame frame, float maxAngleDegrees, float2 random)
{
    float maxRad = radians(maxAngleDegrees);
    float phi = random.x * PI * 2;
    float theta = random.y * maxRad;

    float sinT, cosT;
    sincos(theta, sinT, cosT);

    float sinP, cosP;
    sincos(phi, sinP, cosP);

    return normalize(frame.Direction * cosT + frame.Right * sinT * cosP + frame.Forward * sinT * sinP);
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
    
    float linearDepth = LinearizeDepth(depth);
    float2 noiseUV = pixelCoord.xy * InvTargetSize.yy * 20 + Random;

    ConeFrame coneFrame = GetConeFrame(lightDir);

    float sum = 0;
    for (uint i = 1; i <= SamplesCount; ++i)
    {
        float2 random = BlueNoise.Sample(sampler_BlueNoise, noiseUV * i) * 2 - 1;

        float sunAngularSize = 0.5;
        float3 rayDir = ConePerturb(coneFrame, sunAngularSize, random);

        RayDesc ray;
        ray.Origin = worldPos + (rayDir + normal) * linearDepth * 0.005;
        ray.Direction = rayDir;
        ray.TMin = 0.01;
        ray.TMax = ShadowsDistance;

        RayQuery<RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH> query;
        query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
        query.Proceed();

        if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
            sum += 1.0;
    }
    
    return sum / SamplesCount;
}