#ifndef ATTRIBUTES
#define ATTRIBUTES

layout(location = 0) in vec4 vertPositionOS;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormalOS;
layout(location = 3) in vec2 texCoord;

layout(std140) uniform Matrices// 128 bytes
{
    mat4 projMatrix;
    mat4 viewMatrix;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

#endif // ATTRIBUTES