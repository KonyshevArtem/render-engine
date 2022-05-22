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
uniform sampler2D _SmoothnessMask;
uniform sampler2D _MetallnessMask;
uniform float _Smoothness;
uniform float _Metallness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMap_ST;
uniform float _NormalIntensity;
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
    vec3 normalWS = sampleNormalWS(vars.NormalWS, vars.TangentWS, vars.UV);
    float smoothness = sampleSmoothness(vars.UV);
    float metallness = sampleMetallness(vars.UV);
    //vec3 reflection = sampleReflection(normalWS, vars.PositionWS.xyz, vars.UV);

    vec4 albedo = texture(_Albedo, vars.UV * _Albedo_ST.zw + _Albedo_ST.xy);

    #ifdef _VERTEX_LIGHT
    vec4 light = vars.Color;
    #else
    LightData lightData = getLightPBR(vars.PositionWS.xyz, normalWS, albedo.rgb, smoothness, metallness);
    #endif

    outColor = vec4(albedo.rgb * lightData.Diffuse + lightData.Specular, albedo.a);
}