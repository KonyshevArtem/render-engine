in Varyings
{
    vec3 PositionWS;
    vec3 NormalWS;
#ifdef _NORMAL_MAP
    vec3 TangentWS;
#endif
    vec2 UV;
} vars;

layout(std140) uniform PerMaterialData
{
    vec4 _Albedo_ST;
#ifndef _DATA_MAP
    float _Roughness;
    float _Metallness;
#endif
#ifdef _NORMAL_MAP
    vec4 _NormalMap_ST;
    float _NormalIntensity;
#endif
#ifdef _REFLECTION
    float _ReflectionCubeLevels;
#endif
};

uniform sampler2D _Albedo;

#ifdef _DATA_MAP
uniform sampler2D _Data;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
#endif

#ifdef _REFLECTION
uniform samplerCube _ReflectionCube;
#endif

#include "../common/camera_data.cg"
#include "../common/lighting.cg"

out vec4 outColor;

void main(){
    #ifdef _NORMAL_MAP
    vec3 normalTS = texture(_NormalMap, vars.UV * _NormalMap_ST.zw + _NormalMap_ST.xy).rgb;
    vec3 normalWS = unpackNormal(normalTS, vars.NormalWS, vars.TangentWS, _NormalIntensity);
    #else
    vec3 normalWS = normalize(vars.NormalWS);
    #endif

    #ifdef _DATA_MAP
    vec4 data = texture(_Data, vars.UV);
    float metallness = data.r;
    float roughness = data.g;
    #else
    float metallness = _Metallness;
    float roughness = _Roughness;
    #endif

    vec4 albedo = texture(_Albedo, vars.UV * _Albedo_ST.zw + _Albedo_ST.xy);

    vec3 finalColor = getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, roughness, metallness);
    outColor = vec4(finalColor, albedo.a);
}