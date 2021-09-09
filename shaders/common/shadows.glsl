#ifndef SHADOWS
#define SHADOWS

#include "attributes.glsl"
#include "camera_data.glsl"

#ifdef _RECEIVE_SHADOWS
uniform sampler2D _ShadowMap;
uniform mat4x4 _LightViewProjMatrix;
#endif

float linearizeDepth(float depth){
    float z = depth * 2 - 1;
    z = 2 * _NearClipPlane * _FarClipPlane / (_FarClipPlane + _NearClipPlane - z * (_FarClipPlane - _NearClipPlane));
    return z / _FarClipPlane;
}

float getSpotLightShadowTerm(VARYINGS varyings, float lightAngleCos)
{
    float shadowFactor = 1;
    #ifdef _RECEIVE_SHADOWS
    vec3 perspectivePosLS = varyings.PositionLS.xyz / varyings.PositionLS.w;
    vec3 shadowCoord = perspectivePosLS * 0.5 + 0.5;
    float depth = texture(_ShadowMap, shadowCoord.xy).x;
    float bias = max(0.05 * (1 - lightAngleCos), 0.005);
    if (linearizeDepth(depth) < linearizeDepth(shadowCoord.z) - bias)
        shadowFactor = 0;
    #endif
    return shadowFactor;
}

#endif