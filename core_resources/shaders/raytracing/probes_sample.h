#ifndef PROBES_SAMPLE_H
#define PROBES_SAMPLE_H

#include "../common/global_defines.h"
#include "probes_common.h"

Texture2D<float4> ProbeLightAtlas : register(PROBES_LIGHT_ATLAS);
Texture2D<float2> ProbeDepthAtlas : register(PROBES_DISTANCES_ATLAS);

SamplerState ProbeAtlasSampler : register(PROBES_ATLAS_SAMPLER);

float3 SampleProbeGI(float3 posWS, float3 normalWS)
{
    float3 lightSum = float3(0, 0, 0);
    float weightSum = 0;

    posWS += normalWS * ProbesData.ProbeSpacing * 0.1;

    float3 localGridPos = WorldPosToLocalGridPos(posWS);
    int3 baseLocalGridIndex = WorldPosToLocalGridIndex(posWS);
    float3 alpha = saturate(localGridPos - float3(baseLocalGridIndex));

    for (uint i = 0; i < 8; ++i)
    {
        int3 gridIndexOffset = int3(i, i >> 1, i >> 2) & int3(1, 1, 1);
        int3 localGridIndex = clamp(baseLocalGridIndex + gridIndexOffset, 0, int3(ProbesData.ProbeGridSize) - 1);
        uint probeIndex = FlattenProbeGridIndex(LocalToStorageGridIndex(localGridIndex));

        float3 probeWorldPos = GetProbeWorldPositionFromLocal(localGridIndex);
        float3 toProbe = normalize(probeWorldPos - posWS);

        uint2 atlasCoord = GetAtlasPixelCoord(probeIndex);
        float2 localCoord = OctahedralToUV(normalWS) * ProbesData.ProbeLightSize;

        float4 lightAndBackfaceRatio = ProbeLightAtlas.SampleLevel(ProbeAtlasSampler, (atlasCoord + localCoord) * ProbesData.InvProbeAtlasSize, 0);
        if (IsProbeBlocked(lightAndBackfaceRatio.w))
            continue;

        float weight = 1;

        float normalWeight = max(0.0001, (dot(toProbe, normalWS) + 1.0) * 0.5);
        weight *= normalWeight * normalWeight + 0.2;

        // Moment visibility test
        {
            float distToProbe = length(probeWorldPos - posWS);

            float2 distancesLocalCoord = OctahedralToUV(-toProbe) * ProbesData.ProbeLightSize;
            float2 distances = ProbeDepthAtlas.SampleLevel(ProbeAtlasSampler, (atlasCoord + distancesLocalCoord) * ProbesData.InvProbeAtlasSize, 0).xy;
            float mean = distances.x;
            float variance = abs(distances.x * distances.x - distances.y);

            float diff = max(distToProbe - mean, 0.0);
            float chebyshevWeight = variance / (variance + diff * diff);

            chebyshevWeight = max(chebyshevWeight * chebyshevWeight, 0.0);
            weight *= (distToProbe <= mean) ? 1.0 : chebyshevWeight;
        }

        weight = max(0.0001, weight);

        float crushThreshold = 0.2;
        if (weight < crushThreshold)
            weight *= weight * weight * (1.0 / (crushThreshold * crushThreshold)); 

        float3 trilinear = lerp(1 - alpha, alpha, gridIndexOffset);
        weight *= trilinear.x * trilinear.y * trilinear.z;

        lightSum += lightAndBackfaceRatio.xyz * weight;
        weightSum += weight;
    }

    if (weightSum > 0.001)
        lightSum /= weightSum;

    return lightSum;
}

#endif