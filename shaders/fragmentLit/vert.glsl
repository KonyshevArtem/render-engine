#version 410
#include "../attributes.glsl"

smooth out vec4 positionWS;
smooth out vec3 normalWS;
smooth out vec4 color;
smooth out vec2 uv;

void main(){
    positionWS = modelMatrix * vertPositionOS;
    gl_Position = projMatrix * viewMatrix * positionWS;
    normalWS = normalize((modelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor;
    uv = texCoord;
}