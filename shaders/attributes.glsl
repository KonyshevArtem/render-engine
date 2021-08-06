#ifndef ATTRIBUTES
#define ATTRIBUTES

layout(location = 0) in vec4 vertPositionOS;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormalOS;
layout(location = 3) in vec2 texCoord;

uniform mat4 _ModelMatrix;
uniform mat4 _ModelNormalMatrix;

#endif // ATTRIBUTES