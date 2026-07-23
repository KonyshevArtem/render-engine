#include "raytracing_common.h"
#include "probes_common.h"
#include "../common/lighting.h"

RWTexture2D<float3> OutProbeTempLightAtlas : register(u0);
RWTexture2D<float> OutProbeTempDepthAtlas : register(u1);

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (any(dtid.xy >= uint2(ProbesData.ProbesUpdatePerFrame, 1) * ProbesData.ProbeLightSize))
        return;

    uint2 localPixelCoord = dtid.xy % ProbesData.ProbeLightSize;
    uint tempProbeIndex = dtid.x / ProbesData.ProbeLightSize.x;
    uint globalProbeIndex = tempProbeIndex + ProbesData.UpdateProbeBaseIndex;
    uint3 probeGridIndex = ProbeIndexToGridIndex(globalProbeIndex);

    float2 uv = (float2(localPixelCoord) + 0.5) / ProbesData.ProbeLightSize;
    float3 direction = UVtoOctahedral(uv);
    float3 probeWorldPos = GetProbeWorldPosition(probeGridIndex);

    RayDesc ray;
    ray.Origin = probeWorldPos;
    ray.Direction = direction;
    ray.TMin = 0.01;
    ray.TMax = ProbesData.ProbeSpacing * 10;

    RayQuery<RAY_FLAG_FORCE_OPAQUE> query;

    query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
    while (query.Proceed()){}

    float3 light;
    float3 indirectLight;
    float distance;
    if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        if (query.CommittedTriangleFrontFace())
        {
            float3 hitPos = ray.Origin + ray.Direction * query.CommittedRayT();
            float3 hitNormal = GetHitWorldNormal(query.CommittedInstanceIndex(), query.CommittedPrimitiveIndex(), query.CommittedWorldToObject3x4());
            
            light = getLightPBR(hitPos, hitNormal, float3(0.9, 0.9, 0.9) / PI, 1, 0, probeWorldPos);
            distance = min(ProbesData.ProbeSpacing * 1.5, query.CommittedRayT() - 0.01);
        }
        else
        {
            light = float3(0, 0, 0);
            distance = min(ProbesData.ProbeSpacing * 1.5, query.CommittedRayT() - 0.01);
        }
    }
    else
    {
        light = SampleReflectionCube(direction, 0);
        distance = ProbesData.ProbeSpacing * 1.5;
    }

    uint2 pixelCoord = GetAtlasPixelCoord(tempProbeIndex, false) + localPixelCoord;
    OutProbeTempLightAtlas[pixelCoord] = light;
    OutProbeTempDepthAtlas[pixelCoord] = distance;
}