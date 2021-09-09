#include "common/attributes.glsl"
#include "common/input.glsl"
#include "common/camera_data.glsl"
#include "common/lighting.glsl"

#define VERTEX vert
#define FRAGMENT frag

VARYINGS vert(){
    VARYINGS vars;

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
    vars.PositionLS = _LightViewProjMatrix * vec4(vars.PositionWS.xyz, 1);
    #endif

    return vars;
}

vec4 frag(VARYINGS varyings){
    #ifdef _VERTEX_LIGHT
    vec4 light = varyings.Color;
    #else
    vec4 light = getLight(varyings);
    #endif

    return texture(_Albedo, varyings.UV * _AlbedoST.zw + _AlbedoST.xy) * light;
}