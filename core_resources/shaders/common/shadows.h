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
    ShadowData _DirLightShadow;
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

Texture2D<float> _DirLightShadowMap : register(t0);
SamplerState sampler_DirLightShadowMap : register(s0);

Texture2DArray<float> _SpotLightShadowMapArray : register(t1);
SamplerState sampler_SpotLightShadowMapArray : register(s1);

Texture2DArray<float> _PointLightShadowMapArray : register(t2);
SamplerState sampler_PointLightShadowMapArray : register(s2);

bool isFragVisible(float fragZ)
{
    return fragZ >= 0 && fragZ <= 1;
}

float getShadowTerm(float fragZ, float shadowMapDepth, float lightAngleCos)
{
    const float biasValue = 0.002;

    float bias = max(biasValue * (1 - lightAngleCos), biasValue);
    return shadowMapDepth > (fragZ - bias) ? 1 : 0;
}

float getDirLightShadowTerm(float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float4 shadowCoord = mul(_DirLightShadow.LightViewProjMatrix, float4(posWS, 1));
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    float depth = _DirLightShadowMap.Sample(sampler_DirLightShadowMap, shadowCoord.xy).x;
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
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
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
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
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
    #else
    return 1;
    #endif
}

#endif