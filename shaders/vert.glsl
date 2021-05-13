#version 410

layout(location = 0) in vec4 vertPosition;
layout(location = 1) in vec4 vertColor;

uniform vec2 vertexOffset;
uniform mat4 perspectiveMatrix;
uniform mat4 viewMatrix;

smooth out vec4 color;

void main(){
    vec4 cameraSpacePos = vertPosition + vec4(vertexOffset.xy, 0, 0);
    gl_Position = perspectiveMatrix * viewMatrix * cameraSpacePos;
    color = vertColor;
}