layout(location = 0) in vec3 vertPositionOS;
layout(location = 1) in vec3 vertNormalOS;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertTangentOS;

uniform mat4 _ModelMatrix;
uniform mat4 _ModelNormalMatrix;

out Varyings
{
    vec4 PositionWS;
    vec4 PositionCS;
    vec3 NormalWS;
    vec3 TangentWS;
    vec2 UV;
    vec4 Color;
} vars;

#ifdef _SMOOTHNESS
uniform float _Smoothness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMapST;
#endif

#include "../common/camera_data.cg"
#include "../common/lighting.cg"

void main(){
    vars.PositionWS = _ModelMatrix * vec4(vertPositionOS, 1);
    vars.PositionCS = _ProjMatrix * _ViewMatrix * vars.PositionWS;
    vars.NormalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    vars.TangentWS = normalize((_ModelNormalMatrix * vec4(vertTangentOS, 0)).xyz);
    vars.UV = texCoord;

    #ifdef _VERTEX_LIGHT
    vars.Color = vec4(getLight(vars.PositionWS.xyz, vars.NormalWS, _NearClipPlane, _FarClipPlane), 1);
    #else
    vars.Color = vec4(1);
    #endif

    gl_Position = vars.PositionCS;
}