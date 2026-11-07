#include "../common/helper_functions.h"
#include "probes_common.h"

struct Attributes
{
    float3 PositionOS : POSITION;
    float3 NormalOS : NORMAL;
};

struct Varyings
{
    float4 PositionCS : SV_POSITION;
    float3 Normal : TEXCOORD0;
    uint ProbeIndex : TEXCOORD1;
};

Texture2D<float4> ProbeLightAtlas : register(t3);
SamplerState sampler_ProbeLightAtlas : register(s3);

Texture2D<float> Depth : register(t1);

cbuffer DebugData : register(b1)
{
    uint2 MouseCoord;
    float2 InvTargetSize;
};

Varyings vertexMain(Attributes input, uint instanceID : SV_InstanceID)
{
    Varyings output;

    float scale = 0.1;
    uint3 probeGridIndex = ProbeIndexToGridIndex(instanceID);
    float4 worldPos = float4(input.PositionOS * scale + GetProbeWorldPosition(probeGridIndex), 1);

    output.PositionCS = mul(_VPMatrix, worldPos);
    output.Normal = input.NormalOS;
    output.ProbeIndex = instanceID;
    return output;
}

float4 fragmentMain(Varyings input) : SV_Target
{
    float2 atlasUV = GetAtlasPixelCoord(input.ProbeIndex);
    float2 localUV = OctahedralToUV(normalize(input.Normal)) * ProbesData.ProbeLightSize;
    float4 lightAndBackfaceRatio = ProbeLightAtlas.SampleLevel(sampler_ProbeLightAtlas, (atlasUV + localUV) * ProbesData.InvProbeAtlasSize, 0);
    float3 light = lightAndBackfaceRatio.xyz;
    
    if (IsProbeBlocked(lightAndBackfaceRatio.w))
        light = float3(1, 0, 1);

    float depth = Depth.Load(int3(MouseCoord, 0));
    float2 clipPos = PixelToClipPosition(MouseCoord.xy, InvTargetSize);
    float3 worldPos = ClipToWorldPosition(float3(clipPos, depth), _InvVPMatrix);
    
    int3 baseLocalGridIndex = WorldPosToLocalGridIndex(worldPos);
    for (uint i = 0; i < 8; ++i)
    {
        int3 gridIndexOffset = int3(i & 1, (i >> 1) & 1, (i >> 2) & 1);
        int3 localGridIndex = clamp(baseLocalGridIndex + gridIndexOffset, 0, int3(ProbesData.ProbeGridSize) - 1);
        uint probeIndex = FlattenProbeGridIndex(LocalToStorageGridIndex(localGridIndex));
        
        if (probeIndex == input.ProbeIndex)
        {
            light = float3(1, 0, 0);
            break;
        }
    }

    return float4(light, 1);
}