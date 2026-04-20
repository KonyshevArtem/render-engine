#include "common/global_defines.h"
#include "common/lighting.h"
#include "common/camera_data.h"

struct Attributes
{
    float3 positionOS : POSITION;
};

cbuffer RaytracedShadowsData : register(b0)
{    
    float2 _InvTargetSize;
    float2 _Padding;
};

Texture2D<float4> GBuffer0 : register(t0);
Texture2D<float4> GBuffer1 : register(t1);
Texture2D<float> Depth : register(t2);

float4 vertexMain(Attributes attributes) : SV_Position
{
    return float4(attributes.positionOS.xyz, 1);
}

float4 fragmentMain(float4 pixelCoord : SV_Position) : SV_Target
{
    float4 albedoRoughness = GBuffer0.Load(int3(pixelCoord.xy, 0));
    float4 normalMetallic = GBuffer1.Load(int3(pixelCoord.xy, 0));
    float depth = Depth.Load(int3(pixelCoord.xy, 0));
    
    float2 clipPos = float2(pixelCoord.xy) * _InvTargetSize * 2 - 1;
#if SCREEN_UV_UPSIDE_DOWN
    clipPos.y = -clipPos.y;
#endif
    
    float4 worldPos = mul(_InvVPMatrix, float4(clipPos, depth * 2 - 1, 1));
    worldPos /= worldPos.w;
    
    float3 worldNormal = normalMetallic.xyz * 2 - 1;
    float3 finalColor = getLightPBR(worldPos.xyz, worldNormal, albedoRoughness.xyz, albedoRoughness.w, normalMetallic.w, _CameraPosWS);
    
    return float4(finalColor, 1);
}