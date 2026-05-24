#include "../common/global_defines.h"
#include "../common/lighting.h"
#include "../common/camera_data.h"
#include "../common/helper_functions.h"

struct Attributes
{
    float3 positionOS : POSITION;
};

cbuffer DebugData : register(b1)
{
    uint2 MouseCoord;
    float2 InvTargetSize;
};

Texture2D<float4> GBuffer1 : register(t0);
Texture2D<float> Depth : register(t1);

float4 vertexMain(Attributes attributes) : SV_Position
{
    return float4(attributes.positionOS.xyz, 1);
}

float4 fragmentMain(float4 pixelCoord : SV_Position) : SV_Target
{
    float4 normalMetallic = GBuffer1.Load(int3(pixelCoord.xy, 0));
    float depth = Depth.Load(int3(pixelCoord.xy, 0));
    if (depth == 1)
        discard;
    
    float2 clipPos = PixelToClipPosition(pixelCoord.xy, InvTargetSize);
    float3 worldPos = ClipToWorldPosition(float3(clipPos, depth), _InvVPMatrix);
    float3 worldNormal = normalMetallic.xyz * 2 - 1;
    
    float3 light = SampleProbeGI(worldPos, worldNormal);

    return float4(light, 1);
}