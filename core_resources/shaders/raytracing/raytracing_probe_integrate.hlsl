#include "raytracing_common.h"
#include "probes_common.h"

Texture2D<float3> ProbeTempLightAtlas : register(t0);
Texture2D<float> ProbeTempDepthAtlas : register(t1);

RWTexture2D<float4> OutProbeLightAtlas : register(u0);
RWTexture2D<float2> OutProbeDepthAtlas : register(u1);

StructuredBuffer<uint> UpdateProbeIndices : register(t0, space1);

[numthreads(8, 8, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (any(dtid.xy >= uint2(ProbesData.ProbesUpdatePerFrame, 1) * ProbesData.ProbeLightSize))
        return;

    uint2 localPixelCoord = dtid.xy % ProbesData.ProbeLightSize;
    uint tempProbeIndex = dtid.x / ProbesData.ProbeLightSize.x;
    uint globalProbeIndex = UpdateProbeIndices[tempProbeIndex];

    const float distanceExponent = 50.0;

    float3 light = float3(0, 0, 0);
    float2 distances = float2(0, 0);
    float weightSum = 0;
    float distanceWeightSum = 0;
    uint backfaceCount = 0;

    float3 targetDirection = UVtoOctahedral((float2(localPixelCoord) + 0.5) / ProbesData.ProbeLightSize);
    uint2 baseTempPixelCoord = GetAtlasPixelCoord(tempProbeIndex, false);
    for (uint x = 0; x < ProbesData.ProbeLightSize; ++x)
    {
        for (uint y = 0; y < ProbesData.ProbeLightSize; ++y)
        {
            uint2 pixelCoord = baseTempPixelCoord + uint2(x, y);

            float3 radiance = ProbeTempLightAtlas[pixelCoord];
            float distance = ProbeTempDepthAtlas[pixelCoord];
            float3 direction = UVtoOctahedral((float2(x, y) + 0.5) / ProbesData.ProbeLightSize);

            float weight = max(0, dot(direction, targetDirection));
            
            bool backface = distance < 0;
            if (!backface)
            {
                light += radiance * weight;
                weightSum += weight;
            }
            backfaceCount += backface ? 1 : 0;
            distance = abs(distance);

            float distanceWeight = pow(weight, distanceExponent);
            distances += float2(distance, distance * distance) * distanceWeight;
            distanceWeightSum += distanceWeight;
        }
    }

    if (weightSum > 0.001)
        light /= weightSum;

    if (distanceWeightSum > 0.0)
        distances /= distanceWeightSum;
    
    float backfaceRatio = float(backfaceCount) / float(ProbesData.ProbeLightSize * ProbesData.ProbeLightSize);

    uint2 pixelCoord = GetAtlasPixelCoord(globalProbeIndex) + localPixelCoord;
    OutProbeLightAtlas[pixelCoord] = float4(light, backfaceRatio);
    OutProbeDepthAtlas[pixelCoord] = distances;
}