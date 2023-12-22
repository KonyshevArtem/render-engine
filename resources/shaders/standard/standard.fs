in Varyings
{
    vec3 PositionWS;
    vec3 NormalWS;
#ifdef _NORMAL_MAP
    vec3 TangentWS;
#endif
    vec2 UV;
} vars;

uniform sampler2D _Albedo;
uniform vec4 _Albedo_ST;

#ifdef _DATA_MAP
uniform sampler2D _Data;
#else
uniform float _Roughness;
uniform float _Metallness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMap_ST;
uniform float _NormalIntensity;
#endif

#ifdef _REFLECTION
uniform samplerCube _ReflectionCube;
uniform float _ReflectionCubeLevels;
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

    //vec3 reflection = sampleReflection(normalWS, vars.PositionWS.xyz, vars.UV);

    vec4 albedo = texture(_Albedo, vars.UV * _Albedo_ST.zw + _Albedo_ST.xy);

    vec3 finalColor = getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, roughness, metallness);
    outColor = vec4(finalColor, albedo.a);
}