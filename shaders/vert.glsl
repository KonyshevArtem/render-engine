#version 410

layout(location = 0) in vec4 vertPosition;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormal;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightDirection;
uniform vec4 lightColor;
uniform vec4 ambientLight;

smooth out vec4 color;

void main(){
    mat4 mvMatrix = viewMatrix * modelMatrix;
    gl_Position = projMatrix * mvMatrix * vertPosition;

    vec3 viewNormal = normalize((mvMatrix * vec4(vertNormal, 0)).xyz);
    vec3 viewLightDirection = normalize((viewMatrix * vec4(normalize(-lightDirection), 0)).xyz);

    color = vertColor * lightColor * clamp(dot(viewNormal, viewLightDirection), 0, 1) + vertColor * ambientLight;
}