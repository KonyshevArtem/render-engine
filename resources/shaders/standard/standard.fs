in Varyings
{
    vec4 PositionWS;
    vec4 PositionCS;
    vec3 NormalWS;
    vec3 TangentWS;
    vec2 UV;
    vec4 Color;
} vars;

uniform sampler2D _Albedo;
uniform vec4 _Albedo_ST;

#ifdef _SPECULAR
uniform sampler2D _SpecularMask;
uniform float _SpecularStrength;
uniform float _Smoothness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMap_ST;
#endif

#ifdef _REFLECTION
uniform samplerCube _ReflectionCube;
uniform sampler2D _ReflectionMask;
uniform float _ReflectionStrength;
#endif

#include "../common/camera_data.cg"
#include "../common/lighting.cg"

out vec4 outColor;

void main(){
    vec3 normalWS = getNormalWS(vars.NormalWS, vars.TangentWS, vars.UV);

    #ifdef _VERTEX_LIGHT
    vec4 light = vars.Color;
    #else
    vec4 specular = sampleSpecular(vars.UV);
    LightData lightData = getLight(vars.PositionWS.xyz, normalWS, specular);
    #endif

    vec4 diffuse = texture(_Albedo, vars.UV * _Albedo_ST.zw + _Albedo_ST.xy);
    vec3 reflection = sampleReflection(normalWS, vars.PositionWS.xyz, vars.UV);

    outColor = vec4(diffuse.rgb * lightData.Light + lightData.Specular + reflection, diffuse.a);
}