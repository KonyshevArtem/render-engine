#ifndef SHADOWS
#define SHADOWS

#include "graphics_backend_macros.h"

#ifdef _RECEIVE_SHADOWS
struct ShadowData
{
    float4x4 LightViewProjMatrix;
};

struct PointLightShadowData
{
    float4x4 LightViewProjMatrices[6];
    float4 LightPosWS;
};

struct ShadowsStruct
{
    ShadowData _DirLightShadow;
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

bool isFragVisible(float fragZ)
{
    return fragZ >= 0 && fragZ <= 1;
}

float getShadowTerm(float fragZ, float shadowMapDepth, float lightAngleCos)
{
    float bias = max(0.0002 * (1 - lightAngleCos), 0.0002);
    return shadowMapDepth > (fragZ - bias) ? 1 : 0;
}
#endif

float getDirLightShadowTerm(TEXTURE2D_FLOAT_PARAMETER(shadowMap, shadowMapSampler), ShadowsStruct data, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float4 shadowCoord = data._DirLightShadow.LightViewProjMatrix * float4(posWS, 1);
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    float depth = SAMPLE_TEXTURE(shadowMap, shadowMapSampler, shadowCoord.xy).x;
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
    #else
    return 1;
    #endif
}

float getSpotLightShadowTerm(TEXTURE2D_ARRAY_FLOAT_PARAMETER(shadowMap, shadowMapSampler), ShadowsStruct data, int index, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float4 shadowCoord = data._SpotLightShadows[index].LightViewProjMatrix * float4(posWS, 1);
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    shadowCoord = shadowCoord / shadowCoord.w;
    float depth = SAMPLE_TEXTURE_ARRAY(shadowMap, shadowMapSampler, shadowCoord.xy, index).x;
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

float getPointLightShadowTerm(TEXTURE2D_ARRAY_FLOAT_PARAMETER(shadowMap, shadowMapSampler), ShadowsStruct data, int index, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    float3 lightToFrag = posWS - data._PointLightShadows[index].LightPosWS.xyz;
    int slice = getPointLightShadowMapSlice(lightToFrag);

    float4 shadowCoord = data._PointLightShadows[index].LightViewProjMatrices[slice] * float4(posWS, 1);
    #if SCREEN_UV_UPSIDE_DOWN
    shadowCoord.y = 1 - shadowCoord.y;
    #endif
    shadowCoord = shadowCoord / shadowCoord.w;
    float depth = SAMPLE_TEXTURE_ARRAY(shadowMap, shadowMapSampler, shadowCoord.xy, index * 6 + slice).x;
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
    #else
    return 1;
    #endif
}

#endif