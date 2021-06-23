#version 410

layout(location = 0) in vec4 vertPositionOS;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormalOS;

layout(std140) uniform Matrices // 128 bytes
{
    mat4 projMatrix;
    mat4 viewMatrix;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

smooth out vec4 positionWS;
smooth out vec3 normalWS;
smooth out vec4 color;

void main(){
    positionWS = modelMatrix * vertPositionOS;
    gl_Position = projMatrix * viewMatrix * positionWS;
    normalWS = normalize((modelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor;
}