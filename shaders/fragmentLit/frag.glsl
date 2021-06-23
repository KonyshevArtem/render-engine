#version 410

smooth in vec4 positionWS;
smooth in vec3 normalWS;
smooth in vec4 color;

out vec4 outColor;

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

vec4 getLight(){
    vec4 light = vec4(0, 0, 0, 0);

    for (int i = 0; i < lightsCount; ++i)
    {
        if (lights[i].isDirectional){
            vec3 lightDirWS = normalize(-lights[i].posOrDirWS);
            light += lights[i].intensity * clamp(dot(normalWS, lightDirWS), 0, 1);
        }
        else {
            vec3 lightDirWS = normalize(lights[i].posOrDirWS - positionWS.xyz);
            float distance = distance(lights[i].posOrDirWS, positionWS.xyz);
            light += lights[i].intensity * clamp(dot(normalWS, lightDirWS), 0, 1) / (1 + lights[i].attenuation * distance * distance);
        }
    }

    return light;
}

void main(){
    outColor = color * ambientLightColor;
    outColor += color * getLight();
}