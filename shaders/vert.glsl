#version 410

layout(location = 0) in vec4 vertPositionOS;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormalOS;

layout(std140) uniform Matrices
{
    mat4 projMatrix;
    mat4 viewMatrix;
};

layout(std140) struct LightData // 48 bytes
{
    vec3 posOrDirWS;        // 0  16
    vec4 intensity;         // 16 32
    bool isDirectional;     // 32 48 - padding of each struct member is max padding
};

layout(std140) uniform Lighting // 80 bytes (68 bytes round up by 16)
{
    LightData lights[1];        // 0  48
    vec4 ambientLightColor;     // 48 64
    int lightsCount;            // 64 68
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

smooth out vec4 color;

void main(){
    gl_Position = projMatrix * viewMatrix * modelMatrix * vertPositionOS;

    vec3 normalWS = normalize((modelNormalMatrix * vec4(vertNormalOS, 0)).xyz);

    color = vertColor * ambientLightColor;
    for (int i = 0; i < lightsCount; ++i)
    {
        vec3 lightDirWS = normalize(-lights[i].posOrDirWS);
        color += vertColor * lights[i].intensity * clamp(dot(normalWS, lightDirWS), 0, 1);
    }
}