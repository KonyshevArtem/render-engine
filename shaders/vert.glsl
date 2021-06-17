#version 410

layout(location = 0) in vec4 vertPosition;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormal;

layout(std140) uniform Matrices
{
    mat4 projMatrix;
    mat4 viewMatrix;
};

layout(std140) uniform Lighting
{
    vec3 directionalLightDirection;
    vec4 directionalLightColor;
    vec4 ambientLightColor;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

smooth out vec4 color;

void main(){
    gl_Position = projMatrix * viewMatrix * modelMatrix * vertPosition;

    mat4 normalMatrix = viewMatrix * modelNormalMatrix;
    vec3 viewNormal = normalize((normalMatrix * vec4(vertNormal, 0)).xyz);
    vec3 viewLightDirection = normalize((viewMatrix * vec4(normalize(-directionalLightDirection), 0)).xyz);

    vec4 directionalColor = vertColor * directionalLightColor * clamp(dot(viewNormal, viewLightDirection), 0, 1);
    vec4 ambientColor = vertColor * ambientLightColor;
    color = directionalColor + ambientColor;
}