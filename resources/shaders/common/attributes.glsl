#ifndef ATTRIBUTES
#define ATTRIBUTES

layout(location = 0) in vec3 vertPositionOS;
layout(location = 1) in vec3 vertNormalOS;
layout(location = 2) in vec2 texCoord;

uniform mat4 _ModelMatrix;
uniform mat4 _ModelNormalMatrix;

struct Varyings
{
    vec4 PositionWS;
    vec4 PositionCS;
    vec3 NormalWS;
    vec2 UV;
    vec3 CubemapUV;
    vec4 Color;
    #ifdef _RECEIVE_SHADOWS
    vec4 SpotLightShadowCoords[MAX_SPOT_LIGHT_SOURCES];
    #endif
};

#endif // ATTRIBUTES