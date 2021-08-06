#version 410
#include "../attributes.glsl"
#include "../camera_data.glsl"

uniform vec4 _AlbedoST;

smooth out vec4 positionWS;
smooth out vec3 normalWS;
smooth out vec4 color;
smooth out vec2 uv;

void main(){
    positionWS = _ModelMatrix * vertPositionOS;
    gl_Position = _ProjMatrix * _ViewMatrix * positionWS;
    normalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor;
    uv = texCoord * _AlbedoST.zw + _AlbedoST.xy;
}