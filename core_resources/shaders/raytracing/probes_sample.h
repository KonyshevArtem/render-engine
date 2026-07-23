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

    uint3 gridIndex = WorldPosToGridIndex(posWS);
    float3 baseWorldPos = GetProbeWorldPosition(gridIndex);
    float3 alpha = saturate((posWS - baseWorldPos) / ProbesData.ProbeSpacing);

    for (uint i = 0; i < 8; ++i)
    {
        uint3 gridIndexOffset = uint3(i, i >> 1, i >> 2) & uint3(1, 1, 1);
        uint3 probeGridIndex = WrapProbeGridIndex(gridIndex + gridIndexOffset);
        uint probeIndex = FlattenProbeGridIndex(probeGridIndex);

        float3 probeWorldPos = GetProbeWorldPosition(probeGridIndex);
        float3 toProbe = normalize(probeWorldPos - posWS);

        uint2 atlasCoord = GetAtlasPixelCoord(probeIndex);
        float2 localCoord = OctahedralToUV(-normalWS) * ProbesData.ProbeLightSize;
        
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

        float3 light = ProbeLightAtlas.SampleLevel(ProbeAtlasSampler, (atlasCoord + localCoord) * ProbesData.InvProbeAtlasSize, 0).xyz;

        lightSum += light * weight;
        weightSum += weight;
    }

    if (weightSum > 0.001)
        lightSum /= weightSum;
    
    return lightSum * 0.5 * PI;
}

#endif