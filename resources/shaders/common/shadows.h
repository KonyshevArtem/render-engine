#ifndef SHADOWS
#define SHADOWS

#ifdef _RECEIVE_SHADOWS
struct ShadowData
{
    mat4x4 LightViewProjMatrix;
};

struct PointLightShadowData
{
    mat4x4 LightViewProjMatrices[6];
    vec4 LightPosWS;
};

layout(std140) uniform Shadows
{
    ShadowData _DirLightShadow;
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
    PointLightShadowData _PointLightShadows[MAX_POINT_LIGHT_SOURCES];
};

uniform sampler2D _DirLightShadowMap;
uniform sampler2DArray _SpotLightShadowMapArray;
uniform sampler2DArray _PointLightShadowMapArray;

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

float getDirLightShadowTerm(float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    vec4 shadowCoord = _DirLightShadow.LightViewProjMatrix * vec4(posWS, 1);
    float depth = texture(_DirLightShadowMap, shadowCoord.xy).x;
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
    #else
    return 1;
    #endif
}

float getSpotLightShadowTerm(int index, float3 posWS, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    vec4 shadowCoord = _SpotLightShadows[index].LightViewProjMatrix * vec4(posWS, 1);
    shadowCoord = shadowCoord / shadowCoord.w;
    float depth = texture(_SpotLightShadowMapArray, vec3(shadowCoord.xy, index)).x;
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
    vec3 lightToFrag = posWS - _PointLightShadows[index].LightPosWS.xyz;
    int slice = getPointLightShadowMapSlice(lightToFrag);

    vec4 shadowCoord = _PointLightShadows[index].LightViewProjMatrices[slice] * vec4(posWS, 1);
    shadowCoord = shadowCoord / shadowCoord.w;
    float depth = texture(_PointLightShadowMapArray, vec3(shadowCoord.xy, index * 6 + slice)).x;
    return isFragVisible(shadowCoord.z) ? getShadowTerm(shadowCoord.z, depth, lightAngleCos) : 1;
    #else
    return 1;
    #endif
}

#endif