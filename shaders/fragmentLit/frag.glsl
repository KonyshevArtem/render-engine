#version 410

smooth in vec4 positionWS;
smooth in vec3 normalWS;
smooth in vec4 color;
smooth in vec2 uv;

out vec4 outColor;

uniform sampler2D albedo;
uniform float smoothness;

layout(std140) struct LightData // 48 bytes (40 bytes round up by 16)
{
    vec3 posOrDirWS;        // 0  16
    vec4 intensity;         // 16 32
    bool isDirectional;     // 32 36
    float attenuation;      // 36 40
};

layout(std140) uniform Lighting // 176 bytes (168 bytes round up by 16)
{
    LightData lights[3];        // 0   144
    vec4 ambientLightColor;     // 144 160
    int lightsCount;            // 160 164
    float gammaCorrection;      // 164 168
};

layout(std140) uniform CameraData // 16 bytes (12 bytes round up by 16)
{
    vec3 cameraPosWS; // 12 bytes
};

vec3 getLightDirWS(LightData light){
    if (light.isDirectional)
        return normalize(-light.posOrDirWS);
    else
        return normalize(light.posOrDirWS - positionWS.xyz);
}

float getAttenuationTerm(LightData light){
    if (light.isDirectional)
        return 1;
    else
    {
        float distance = distance(light.posOrDirWS, positionWS.xyz);
        return 1 / (1 + light.attenuation * distance * distance);
    }
}

float getSpecularTerm(vec3 lightDirWS, float lightAngleCos, vec3 normal){
    vec3 viewDirWS = normalize(cameraPosWS - positionWS.xyz);
    vec3 halfAngle = normalize(lightDirWS + viewDirWS);
    float blinnTerm = clamp(dot(normal, halfAngle), 0, 1);
    blinnTerm = lightAngleCos != 0 ? blinnTerm : 0;
    blinnTerm = pow(blinnTerm, smoothness);
    return blinnTerm;
}

vec4 getLight(){
    vec4 light = vec4(0, 0, 0, 0);
    vec3 normal = normalize(normalWS);

    for (int i = 0; i < lightsCount; ++i)
    {
        vec3 lightDirWS = getLightDirWS(lights[i]);
        float lightAngleCos = clamp(dot(normal, lightDirWS), 0, 1);
        float attenuationTerm = getAttenuationTerm(lights[i]);
        float specularTerm = getSpecularTerm(lightDirWS, lightAngleCos, normal);
        light += lights[i].intensity * lightAngleCos * attenuationTerm;
        light += lightAngleCos * specularTerm * attenuationTerm;
    }

    return light;
}

void main(){
    vec4 albedoColor = texture(albedo, uv) * color;
    outColor = albedoColor * ambientLightColor;
    outColor += albedoColor * getLight();

    vec4 gamma = vec4(gammaCorrection, gammaCorrection, gammaCorrection, 1);
    outColor = pow(outColor, gamma);
}