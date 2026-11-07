#include "probes_common.h"

RWTexture2D<float4> OutProbeLightAtlas : register(u0);
RWTexture2D<float2> OutProbeDepthAtlas : register(u1);

StructuredBuffer<uint> UpdateProbeIndices : register(t0, space1);

uint2 WrapOctahedral(int2 coord, uint size) 
{
    int2 wrapped = coord;
    if (wrapped.x < 0)        { wrapped.x = -wrapped.x - 1;        wrapped.y = size - 1 - wrapped.y; }
    if (wrapped.x >= size)  { wrapped.x = 2*size - 1 - wrapped.x; wrapped.y = size - 1 - wrapped.y; }
    if (wrapped.y < 0)        { wrapped.y = -wrapped.y - 1;        wrapped.x = size - 1 - wrapped.x; }
    if (wrapped.y >= size)  { wrapped.y = 2*size - 1 - wrapped.y; wrapped.x = size - 1 - wrapped.x; }
    return wrapped;
}

void AddBorderPixel(uint2 atlasCoord, int2 unwrappedCoord)
{
    uint2 srcCoord = atlasCoord + WrapOctahedral(unwrappedCoord, ProbesData.ProbeLightSize);
    uint2 dstCoord = uint2(int2(atlasCoord) + unwrappedCoord);

    OutProbeLightAtlas[dstCoord] = OutProbeLightAtlas[srcCoord];
    OutProbeDepthAtlas[dstCoord] = OutProbeDepthAtlas[srcCoord].xy;
}

[numthreads(64, 1, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (dtid.x >= ProbesData.ProbesUpdatePerFrame)
        return;

    uint probeIndex = UpdateProbeIndices[dtid.x];

    uint2 atlasCoord = GetAtlasPixelCoord(probeIndex);

    for (uint i = 0; i < ProbesData.ProbeLightSize; ++i)
    {
        AddBorderPixel(atlasCoord, int2(-1, i));
        AddBorderPixel(atlasCoord, int2(i, -1));
        AddBorderPixel(atlasCoord, int2(ProbesData.ProbeLightSize, i));
        AddBorderPixel(atlasCoord, int2(i, ProbesData.ProbeLightSize));
    }

    AddBorderPixel(atlasCoord, int2(-1, -1));
    AddBorderPixel(atlasCoord, int2(ProbesData.ProbeLightSize, -1));
    AddBorderPixel(atlasCoord, int2(-1, ProbesData.ProbeLightSize));
    AddBorderPixel(atlasCoord, int2(ProbesData.ProbeLightSize, ProbesData.ProbeLightSize));
}