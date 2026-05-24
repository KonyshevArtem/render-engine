#include "raytracing_common.h"
#include "probes_common.h"
#include "../common/lighting.h"

RWTexture2D<float3> OutProbeLightAtlas : register(u0);

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
    uint2 wrappedCoord = WrapOctahedral(unwrappedCoord, ProbesData.ProbeLightSize);
    float3 light = OutProbeLightAtlas[atlasCoord + wrappedCoord].xyz;
    OutProbeLightAtlas[atlasCoord + unwrappedCoord] = light;
}

[numthreads(64, 1, 1)]
void computeMain(uint3 dtid : SV_DispatchThreadID)
{
    if (dtid.x >= ProbesData.ProbesUpdatePerFrame)
        return;

    uint probeIndex = dtid.x + ProbesData.UpdateProbeBaseIndex;
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