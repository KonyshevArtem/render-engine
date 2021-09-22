#include "common/attributes.glsl"
#include "common/input.glsl"
#include "common/camera_data.glsl"
#include "common/lighting.glsl"

#define VERTEX vert
#define FRAGMENT frag

Varyings vert(){
    Varyings vars;

    vars.PositionWS = _ModelMatrix * vertPositionOS;
    vars.PositionCS = _ProjMatrix * _ViewMatrix * vars.PositionWS;
    vars.NormalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    vars.UV = texCoord;

    #ifdef _VERTEX_LIGHT
    vars.Color = getLight(vars);
    #else
    vars.Color = vec4(1);
    #endif

    #ifdef _RECEIVE_SHADOWS
    transferShadowCoords(vars, _PointLightsCount, _SpotLightsCount);
    #endif

    return vars;
}

vec4 frag(Varyings varyings){
    #ifdef _VERTEX_LIGHT
    vec4 light = varyings.Color;
    #else
    vec4 light = getLight(varyings);
    #endif

    return texture(_Albedo, varyings.UV * _AlbedoST.zw + _AlbedoST.xy) * light;
}