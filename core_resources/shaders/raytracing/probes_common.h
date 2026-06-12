#ifndef PROBES_COMMON_H
#define PROBES_COMMON_H

#include "../common/global_defines.h"

struct ProbeData
{
    uint3 ProbeGridSize;
    float ProbeSpacing;

    uint ProbesPerRow;
    uint ProbesCount;
    uint UpdateProbeBaseIndex;
    uint ProbeLightSize;

    uint ProbesUpdatePerFrame;
    uint ProbeLightPaddedSize;
    float2 InvProbeAtlasSize;
};

ConstantBuffer<ProbeData> ProbesData : register(PROBE_DATA);

float3 GetGridStartPos()
{
    float3 halfGridSize = float3(ProbesData.ProbeGridSize - 1) * ProbesData.ProbeSpacing * 0.5;
    return -halfGridSize;
}

float3 GetProbeWorldPosition(uint probeIndex)
{
    uint3 probeGridPos;
    probeGridPos.x = probeIndex % ProbesData.ProbeGridSize.x;
    probeGridPos.y = (probeIndex / ProbesData.ProbeGridSize.x) % ProbesData.ProbeGridSize.y;
    probeGridPos.z = probeIndex / (ProbesData.ProbeGridSize.x * ProbesData.ProbeGridSize.y);

    return GetGridStartPos() + probeGridPos * ProbesData.ProbeSpacing;
}

uint3 WorldPosToGridIndex(float3 worldPos)
{
    float3 gridPos = (worldPos - GetGridStartPos()) / ProbesData.ProbeSpacing;
    return clamp(uint3(gridPos), 0, ProbesData.ProbeGridSize - 1);
}

uint FlattenProbeGridIndex(uint3 gridIndex)
{
    return gridIndex.x + gridIndex.y * ProbesData.ProbeGridSize.x + gridIndex.z * ProbesData.ProbeGridSize.x * ProbesData.ProbeGridSize.y;
}

uint2 GetAtlasPixelCoord(uint probeIndex, bool addBorder)
{
    uint border = addBorder ? 2 : 0;
    return uint2(probeIndex % ProbesData.ProbesPerRow, probeIndex / ProbesData.ProbesPerRow) * (ProbesData.ProbeLightPaddedSize + border);
}

uint2 GetAtlasPixelCoord(uint probeIndex)
{
    return GetAtlasPixelCoord(probeIndex, true);
}

float SignNotZero(float v)
{
    return (v >= 0) ? 1.0 : -1.0;
}

float3 UVtoOctahedral(float2 uv) 
{
    float2 f = uv * 2.0 - 1.0;
    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));

    if (n.z < 0.0)
        n.xy = (1.0 - abs(n.yx)) * float2(SignNotZero(n.x), SignNotZero(n.y));

    return normalize(n);
}

float2 OctahedralToUV(float3 n) 
{
    float3 absN = abs(n);
    n /= absN.x + absN.y + absN.z;

    float2 uv;
    if (n.z >= 0.0)
        uv = n.xy;
    else
    {
        uv.x = (1.0 - abs(n.y)) * SignNotZero(n.x);
        uv.y = (1.0 - abs(n.x)) * SignNotZero(n.y);
    }

    return uv * 0.5 + 0.5;
}

#endif