#version 410

layout(location = 0) in vec4 vertPositionOS;
layout(location = 1) in vec4 vertColor;
layout(location = 2) in vec3 vertNormalOS;

layout(std140) uniform Matrices
{
    mat4 projMatrix;
    mat4 viewMatrix;
};

layout(std140) struct LightData // 48 bytes (40 bytes round up by 16)
{
    vec3 posOrDirWS;        // 0  16
    vec4 intensity;         // 16 32
    bool isDirectional;     // 32 36
    float attenuation;      // 36 40
};

layout(std140) uniform Lighting // 128 bytes (116 bytes round up by 16)
{
    LightData lights[2];        // 0   96
    vec4 ambientLightColor;     // 96  112
    int lightsCount;            // 112 116
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

smooth out vec4 color;

vec4 getLight(vec3 vertPositionWS){
    vec4 light = vec4(0, 0, 0, 0);
    vec3 normalWS = normalize((modelNormalMatrix * vec4(vertNormalOS, 0)).xyz);

    for (int i = 0; i < lightsCount; ++i)
    {
        if (lights[i].isDirectional){
            vec3 lightDirWS = normalize(-lights[i].posOrDirWS);
            light += lights[i].intensity * clamp(dot(normalWS, lightDirWS), 0, 1);
        }
        else {
            vec3 lightDirWS = normalize(lights[i].posOrDirWS - vertPositionWS);
            float distance = distance(lights[i].posOrDirWS, vertPositionWS);
            light += lights[i].intensity * clamp(dot(normalWS, lightDirWS), 0, 1) / (1 + lights[i].attenuation * distance * distance);
        }
    }

    return light;
}

void main(){
    vec4 vertPositionWS = modelMatrix * vertPositionOS;
    gl_Position = projMatrix * viewMatrix * vertPositionWS;

    color = vertColor * ambientLightColor;
    color += vertColor * getLight(vertPositionWS.xyz);
}