#ifndef SHADOWS_H
#define SHADOWS_H

#include "global_defines.h"

#ifdef _RAYTRACED_SHADOWS
#include "../raytracing/raytracing_shadows.h"
#endif

struct ShadowData
{
    float4x4 LightViewProjMatrix;
};

struct SpotLightShadowData
{
    float4x4 LightViewProjMatrix;

    float3 Padding0;
    uint ShadowAtlasSlot;
};

struct PointLightShadowData
{
    float4x4 LightViewProjMatrices[6];
    float4 LightPosWS;

    uint4 ShadowAtlasSlots[2];
};

cbuffer Shadows : register(SHADOW_DATA)
{
    ShadowData _DirLightShadow[SHADOW_CASCADE_COUNT];
    SpotLightShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

#ifdef _RAYTRACED_SHADOWS
Texture2D<float> _RaytracedShadowMask : register(RAYTRACED_SHADOW_MASK);
#else
Texture2DArray<float> _DirLightShadowMap : register(DIRECTIONAL_SHADOW_MAP);
SamplerComparisonState sampler_DirLightShadowMap : register(DIRECTIONAL_SHADOW_MAP_SAMPLER);
#endif

Texture2DArray<float> _PunctualLightShadowAtlas : register(PUNCTUAL_LIGHT_SHADOW_ATLAS);
SamplerComparisonState sampler_PunctualLightShadowAtlas : register(PUNCTUAL_LIGHT_SHADOW_ATLAS_SAMPLER);

bool isFragVisibleZ(float fragZ)
{
    return fragZ >= 0 && fragZ <= 1;
}

bool isFragVisibleXY(float2 fragXY)
{
    return all(fragXY >= 0) && all(fragXY <= 1);
}

float getDirLightShadowTerm(float3 posWS, float3 normalWS, float3 lightDir, uint2 pixelCoord, bool isOpaque)
{
#if defined(_RECEIVE_SHADOWS)
    #if defined(_RAYTRACED_SHADOWS)
    if (isOpaque)
        return 1.0 - _RaytracedShadowMask.Load(uint3(pixelCoord, 0)).x;
    else
        return 1.0 - TraceShadowRay(posWS, normalWS, lightDir, 50);
    #else
    float3 shadowCoord;
    int cascadeIndex = -1;
    for (int i = 0; i < SHADOW_CASCADE_COUNT; ++i)
    {
        shadowCoord = mul(_DirLightShadow[i].LightViewProjMatrix, float4(posWS, 1)).xyz;
        if (isFragVisibleXY(shadowCoord.xy))
        {
            cascadeIndex = i;
            break;
        }
    }
    if (cascadeIndex < 0)
        return 1;

    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif

    return _DirLightShadowMap.SampleCmpLevelZero(sampler_DirLightShadowMap, float3(shadowCoord.xy, cascadeIndex), saturate(shadowCoord.z)).x;
    #endif
#else
    return 1;
#endif
}

float getSpotLightShadowTerm(int index, float3 posWS)
{
    #ifdef _RECEIVE_SHADOWS
    float4 shadowCoord = mul(_SpotLightShadows[index].LightViewProjMatrix, float4(posWS, 1));
    shadowCoord = shadowCoord / shadowCoord.w;
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif

    uint slot = _SpotLightShadows[index].ShadowAtlasSlot;
    return _PunctualLightShadowAtlas.SampleCmpLevelZero(sampler_PunctualLightShadowAtlas, float3(shadowCoord.xy, slot), saturate(shadowCoord.z)).x;
    #else
    return 1;
    #endif
}

uint getPointLightShadowMapSlice(float3 lightToFrag)
{
    if (abs(lightToFrag.x) > abs(lightToFrag.y) && abs(lightToFrag.x) > abs(lightToFrag.z))
    {
        return lightToFrag.x > 0 ? 0 : 1;
    }

    if (abs(lightToFrag.y) > abs(lightToFrag.x) && abs(lightToFrag.y) > abs(lightToFrag.z))
    {
        return lightToFrag.y > 0 ? 2 : 3;
    }

    return lightToFrag.z > 0 ? 4 : 5;
}

float getPointLightShadowTerm(int index, float3 posWS)
{
    #ifdef _RECEIVE_SHADOWS
    float3 lightToFrag = posWS - _PointLightShadows[index].LightPosWS.xyz;
    uint slice = getPointLightShadowMapSlice(lightToFrag);

    float4 shadowCoord = mul(_PointLightShadows[index].LightViewProjMatrices[slice], float4(posWS, 1));
    shadowCoord = shadowCoord / shadowCoord.w;
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif

    uint slot = _PointLightShadows[index].ShadowAtlasSlots[slice >> 2][slice & 3];
    return _PunctualLightShadowAtlas.SampleCmpLevelZero(sampler_PunctualLightShadowAtlas, float3(shadowCoord.xy, slot), saturate(shadowCoord.z)).x;
    #else
    return 1;
    #endif
}

#endif // SHADOWS_H