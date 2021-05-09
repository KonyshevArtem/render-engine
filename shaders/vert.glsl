#version 410

layout(location = 0) in vec4 vertPosition;
layout(location = 1) in vec4 vertColor;

uniform vec2 vertexOffset;

smooth out vec4 color;

void main(){
    gl_Position = vertPosition + vec4(vertexOffset.xy, 0, 0);
    color = vertColor;
}