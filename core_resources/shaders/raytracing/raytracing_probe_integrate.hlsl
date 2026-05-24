#include "raytracing_common.h"
#include "probes_common.h"
#include "../common/lighting.h"

Texture2D<float3> ProbeTempAtlas : register(t0);
RWTexture2D<float3> OutProbeLightAtlas : register(u0);

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 localPixelCoord = dtid.xy % ProbesData.ProbeLightSize;
    uint tempProbeIndex = dtid.x / ProbesData.ProbeLightSize.x;
    uint globalProbeIndex = tempProbeIndex + ProbesData.UpdateProbeBaseIndex;

    float3 light = float3(0, 0, 0);
    float weightSum = 0;

    float3 targetDirection = UVtoOctahedral((float2(localPixelCoord) + 0.5) / ProbesData.ProbeLightSize);
    uint2 baseTempPixelCoord = GetAtlasPixelCoord(tempProbeIndex);
    for (uint x = 0; x < ProbesData.ProbeLightSize; ++x)
    {
        for (uint y = 0; y < ProbesData.ProbeLightSize; ++y)
        {
            float3 radiance = ProbeTempAtlas[baseTempPixelCoord + uint2(x, y)] * 0.95;
            float3 direction = UVtoOctahedral((float2(x, y) + 0.5) / ProbesData.ProbeLightSize);

            float weight = max(0, dot(direction, targetDirection));
            light += radiance * weight;
            weightSum += weight;
        }
    }

    if (weightSum > 0.001)
        light /= weightSum;

    OutProbeLightAtlas[GetAtlasPixelCoord(globalProbeIndex) + localPixelCoord] = light;
}