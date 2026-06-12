#include "raytracing_common.h"
#include "probes_common.h"

Texture2D<float3> ProbeTempLightAtlas : register(t0);
Texture2D<float> ProbeTempDepthAtlas : register(t1);

RWTexture2D<float3> OutProbeLightAtlas : register(u0);
RWTexture2D<float2> OutProbeDepthAtlas : register(u1);

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (any(dtid.xy >= uint2(ProbesData.ProbesUpdatePerFrame, 1) * ProbesData.ProbeLightSize))
        return;

    uint2 localPixelCoord = dtid.xy % ProbesData.ProbeLightSize;
    uint tempProbeIndex = dtid.x / ProbesData.ProbeLightSize.x;
    uint globalProbeIndex = tempProbeIndex + ProbesData.UpdateProbeBaseIndex;

    float3 light = float3(0, 0, 0);
    float2 distances = float2(0, 0);
    float weightSum = 0;

    float3 targetDirection = UVtoOctahedral((float2(localPixelCoord) + 0.5) / ProbesData.ProbeLightSize);
    uint2 baseTempPixelCoord = GetAtlasPixelCoord(tempProbeIndex, false);
    for (uint x = 0; x < ProbesData.ProbeLightSize; ++x)
    {
        for (uint y = 0; y < ProbesData.ProbeLightSize; ++y)
        {
            uint2 pixelCoord = baseTempPixelCoord + uint2(x, y);

            float3 radiance = ProbeTempLightAtlas[pixelCoord] * 0.95;
            float distance = ProbeTempDepthAtlas[pixelCoord];
            float3 direction = UVtoOctahedral((float2(x, y) + 0.5) / ProbesData.ProbeLightSize);

            float weight = max(0, dot(direction, targetDirection));
            light += radiance * weight;
            distances += float2(distance, distance * distance) * weight * weight;
            weightSum += weight;
        }
    }

    if (weightSum > 0.001)
    {
        light /= weightSum;
        distances /= weightSum;
    }

    uint2 pixelCoord = GetAtlasPixelCoord(globalProbeIndex) + localPixelCoord;
    OutProbeLightAtlas[pixelCoord] = light;
    OutProbeDepthAtlas[pixelCoord] = distances;
}