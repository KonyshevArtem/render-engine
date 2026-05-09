#ifndef RAYTRACED_SHADOWS_H
#define RAYTRACED_SHADOWS_H

#include "../common/global_defines.h"
#include "../common/camera_data.h"
#include "raytracing_common.h"

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

float TraceShadowRay(float3 worldPos, float3 worldNormal, float3 lightDir, float maxDistance
#ifdef RAYTRACED_SOFT_SHADOWS
    , uint samplesCount, Texture2D<float2> noiseTex, SamplerState noiseSampler, float2 noiseUV
#endif
)
{
    if (dot(worldNormal, lightDir) <= 0)
        return 0;

    float cameraDist = distance(worldPos, _CameraPosWS);

    float sum = 0;

#ifdef RAYTRACED_SOFT_SHADOWS
    ConeFrame coneFrame = GetConeFrame(lightDir);
    for (uint i = 1; i <= samplesCount; ++i)
    {
        float2 random = noiseTex.Sample(noiseSampler, noiseUV * i) * 2 - 1;

        float sunAngularSize = 0.5;
        float3 rayDir = ConePerturb(coneFrame, sunAngularSize, random);
#else
    uint samplesCount = 1;
    float3 rayDir = lightDir;
    {
#endif

        RayDesc ray;
        ray.Origin = worldPos + (rayDir + worldNormal) * cameraDist * 0.005;
        ray.Direction = rayDir;
        ray.TMin = 0.01;
        ray.TMax = maxDistance;

        RayQuery<RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH> query;
        query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
        query.Proceed();

        if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
            sum += 1.0;
    }
    
    return sum / samplesCount;
}

#endif