#include "raytracing_common.h"
#include "probes_common.h"
#include "../common/lighting.h"

RWTexture2D<float3> OutProbeTempAtlas : register(u0);

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 localPixelCoord = dtid.xy % ProbesData.ProbeLightSize;
    uint tempProbeIndex = dtid.x / ProbesData.ProbeLightSize.x;
    uint globalProbeIndex = tempProbeIndex + ProbesData.UpdateProbeBaseIndex;

    float2 uv = (float2(localPixelCoord) + 0.5) / ProbesData.ProbeLightSize;
    float3 direction = UVtoOctahedral(uv);
    float3 probeWorldPos = GetProbeWorldPosition(globalProbeIndex);

    RayDesc ray;
    ray.Origin = probeWorldPos;
    ray.Direction = direction;
    ray.TMin = 0.01;
    ray.TMax = ProbesData.ProbeSpacing * 10;

    RayQuery<RAY_FLAG_FORCE_OPAQUE> query;

    query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
    while (query.Proceed()){}

    float3 light;
    if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        if (query.CommittedTriangleFrontFace())
        {
            float3 hitPos = ray.Origin + ray.Direction * query.CommittedRayT();
            float3 hitNormal = GetHitWorldNormal(query.CommittedInstanceIndex(), query.CommittedPrimitiveIndex(), query.CommittedWorldToObject3x4());
            
            light = getLightPBR(hitPos, hitNormal, float3(1, 1, 1), 1, 0, probeWorldPos);
        }
        else
            light = float3(0, 0, 0);
    }
    else
        light = SampleReflectionCube(direction, 0);

    OutProbeTempAtlas[GetAtlasPixelCoord(tempProbeIndex) + localPixelCoord] = light;
}