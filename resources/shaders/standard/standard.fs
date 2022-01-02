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
uniform vec4 _AlbedoST;

#ifdef _SMOOTHNESS
uniform float _Smoothness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMapST;
#endif

#include "../common/camera_data.cg"
#include "../common/lighting.cg"

out vec4 outColor;

void main(){
    #ifdef _VERTEX_LIGHT
    vec4 light = vars.Color;
    #else
    vec3 normalWS = getNormalWS(vars.NormalWS, vars.TangentWS, vars.UV);
    vec4 light = vec4(getLight(vars.PositionWS.xyz, normalWS, _NearClipPlane, _FarClipPlane), 1);
    #endif

    outColor = texture(_Albedo, vars.UV * _AlbedoST.zw + _AlbedoST.xy) * light;
}