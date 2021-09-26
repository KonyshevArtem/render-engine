#ifndef SHADOWS
#define SHADOWS

#include "camera_data.glsl"

#ifdef _RECEIVE_SHADOWS
layout(std140) struct ShadowData
{
    mat4x4 LightViewProjMatrix;
};

layout(std140) uniform Shadows
{
    ShadowData _SpotLightShadows[MAX_SPOT_LIGHT_SOURCES];
};

uniform sampler2DArray _SpotLightShadowMapArray;
#endif

void transferShadowCoords(inout Varyings vars, int pointLightsCount, int spotLightsCount){
    #ifdef _RECEIVE_SHADOWS
    for (int i = 0; i < spotLightsCount; ++i){
        vars.SpotLightShadowCoords[i] = _SpotLightShadows[i].LightViewProjMatrix * vec4(vars.PositionWS.xyz, 1);
    }
    #endif
}

float linearizeDepth(float depth){
    float z = depth * 2 - 1;
    z = 2 * _NearClipPlane * _FarClipPlane / (_FarClipPlane + _NearClipPlane - z * (_FarClipPlane - _NearClipPlane));
    return z / _FarClipPlane;
}

float getSpotLightShadowTerm(Varyings vars, int index, float lightAngleCos)
{
    #ifdef _RECEIVE_SHADOWS
    vec3 shadowCoord = vars.SpotLightShadowCoords[index].xyz / vars.SpotLightShadowCoords[index].w;
    shadowCoord = shadowCoord * 0.5 + 0.5;
    float depth = texture(_SpotLightShadowMapArray, vec3(shadowCoord.xy, index)).x;
    float bias = max(0.05 * (1 - lightAngleCos), 0.005);
    if (linearizeDepth(depth) < linearizeDepth(shadowCoord.z) - bias)
        return 0;
    #endif
    return 1;
}

#endif