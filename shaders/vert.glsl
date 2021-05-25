#version 410

layout(location = 0) in vec4 vertPosition;
layout(location = 1) in vec4 vertColor;

uniform mat4 matrix;

smooth out vec4 color;

void main(){
    gl_Position = matrix * vertPosition;
    color = vertColor;
}