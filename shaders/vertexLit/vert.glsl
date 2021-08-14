#version 410
#include "../attributes.glsl"
#include "../camera_data.glsl"
#include "../lighting.glsl"

uniform vec4 _AlbedoST;

smooth out vec4 color;
smooth out vec2 uv;

void main(){
    vec4 vertPositionWS = _ModelMatrix * vertPositionOS;
    gl_Position = _ProjMatrix * _ViewMatrix * vertPositionWS;

    vec3 normalWS = normalize((_ModelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor * _AmbientLightColor;
    color += vertColor * getLight(vertPositionWS.xyz, normalWS, false, 0, vec3(0, 0, 0));

    uv = texCoord * _AlbedoST.zw + _AlbedoST.xy;
}