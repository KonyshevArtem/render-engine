#ifndef SHADOWS
#define SHADOWS

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

cbuffer Shadows : register(b1)
{
    ShadowData _DirLightShadow[SHADOW_CASCADE_COUNT];
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

Texture2DArray<float> _DirLightShadowMap : register(t0);
SamplerState sampler_DirLightShadowMap : register(s0);

Texture2DArray<float> _SpotLightShadowMapArray : register(t1);
SamplerState sampler_SpotLightShadowMapArray : register(s1);

Texture2DArray<float> _PointLightShadowMapArray : register(t2);
SamplerState sampler_PointLightShadowMapArray : register(s2);

bool isFragVisibleZ(float fragZ)
{
    return fragZ >= 0 && fragZ <= 1;
}

bool isFragVisibleXY(float2 fragXY)
{
    return all(fragXY >= 0) && all(fragXY <= 1);
}

float getShadowTerm(float fragZ, float shadowMapDepth, float lightAngleCos, bool isLinearDepth)
{
    const float depthBias = 0.005;
    const float slopeDepthBias = 0.05;

    float bias = lerp(slopeDepthBias, depthBias, saturate(abs(lightAngleCos)));

    float biasedFragZ;
    if (isLinearDepth)
        biasedFragZ = fragZ - bias;
    else
        biasedFragZ = fragZ - (1 - fragZ) * bias;

    return shadowMapDepth > biasedFragZ ? 1 : 0;
}

float getDirLightShadowTerm(float3 posWS, float lightAngleCos, float fragDistance)
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
    float depth = _DirLightShadowMap.Sample(sampler_DirLightShadowMap, float3(shadowCoord.xy, cascadeIndex)).x;
    return getShadowTerm(saturate(shadowCoord.z), depth, lightAngleCos, true);
    #else
    return 1;
    #endif
}

float getSpotLightShadowTerm(int index, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float4 shadowCoord = mul(_SpotLightShadows[index].LightViewProjMatrix, float4(posWS, 1));
    shadowCoord = shadowCoord / shadowCoord.w;
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    float depth = _SpotLightShadowMapArray.Sample(sampler_SpotLightShadowMapArray, float3(shadowCoord.xy, index)).x;
    return isFragVisibleZ(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos, false) : 1;
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

float getPointLightShadowTerm(int index, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float3 lightToFrag = posWS - _PointLightShadows[index].LightPosWS.xyz;
    int slice = getPointLightShadowMapSlice(lightToFrag);

    float4 shadowCoord = mul(_PointLightShadows[index].LightViewProjMatrices[slice], float4(posWS, 1));
    shadowCoord = shadowCoord / shadowCoord.w;
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    float depth = _PointLightShadowMapArray.Sample(sampler_PointLightShadowMapArray, float3(shadowCoord.xy, index * 6 + slice)).x;
    return isFragVisibleZ(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos, false) : 1;
    #else
    return 1;
    #endif
}

#endif