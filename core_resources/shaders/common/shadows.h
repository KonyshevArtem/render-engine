#ifndef SHADOWS_H
#define SHADOWS_H

#include "global_defines.h"

struct ShadowData
{
    float4x4 LightViewProjMatrix;
};

struct PointLightShadowData
{
    float4x4 LightViewProjMatrices[6];
    float4 LightPosWS;
};

cbuffer Shadows : register(SHADOW_DATA)
{
    ShadowData _DirLightShadow[SHADOW_CASCADE_COUNT];
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

Texture2DArray<float> _DirLightShadowMap : register(DIRECTIONAL_SHADOW_MAP);
SamplerComparisonState sampler_DirLightShadowMap : register(DIRECTIONAL_SHADOW_MAP_SAMPLER);

Texture2DArray<float> _SpotLightShadowMapArray : register(SPOTLIGHT_SHADOW_MAP);
SamplerComparisonState sampler_SpotLightShadowMapArray : register(SPOTLIGHT_SHADOW_MAP_SAMPLER);

Texture2DArray<float> _PointLightShadowMapArray : register(POINTLIGHT_SHADOW_MAP);
SamplerComparisonState sampler_PointLightShadowMapArray : register(POINTLIGHT_SHADOW_MAP_SAMPLER);

bool isFragVisibleZ(float fragZ)
{
    return fragZ >= 0 && fragZ <= 1;
}

bool isFragVisibleXY(float2 fragXY)
{
    return all(fragXY >= 0) && all(fragXY <= 1);
}

float getDirLightShadowTerm(float3 posWS)
{
    #ifdef _RECEIVE_SHADOWS
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

    return _SpotLightShadowMapArray.SampleCmpLevelZero(sampler_SpotLightShadowMapArray, float3(shadowCoord.xy, index), saturate(shadowCoord.z)).x;
    #else
    return 1;
    #endif
}

int getPointLightShadowMapSlice(float3 lightToFrag)
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
    int slice = getPointLightShadowMapSlice(lightToFrag);

    float4 shadowCoord = mul(_PointLightShadows[index].LightViewProjMatrices[slice], float4(posWS, 1));
    shadowCoord = shadowCoord / shadowCoord.w;
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif

    index = index * 6 + slice; // keep this on a separate line, otherwise SPIRV-Cross does not declare _PointLightShadowMapArray as sampler2DArrayShadow
    return _PointLightShadowMapArray.SampleCmpLevelZero(sampler_PointLightShadowMapArray, float3(shadowCoord.xy, index), saturate(shadowCoord.z)).x;
    #else
    return 1;
    #endif
}

#endif // SHADOWS_H