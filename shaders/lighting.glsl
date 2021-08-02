#ifndef LIGHTING
#define LIGHTING

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

vec3 getLightDirWS(LightData light, vec3 posWS){
    if (light.isDirectional)
    return normalize(-light.posOrDirWS);
    else
    return normalize(light.posOrDirWS - posWS);
}

float getAttenuationTerm(LightData light, vec3 posWS){
    if (light.isDirectional)
    return 1;
    else
    {
        float distance = distance(light.posOrDirWS, posWS);
        return 1 / (1 + light.attenuation * distance * distance);
    }
}

float getSpecularTerm(float smoothness, vec3 lightDirWS, float lightAngleCos, vec3 posWS, vec3 normalWS, vec3 cameraPosWS){
    vec3 viewDirWS = normalize(cameraPosWS - posWS);
    vec3 halfAngle = normalize(lightDirWS + viewDirWS);
    float blinnTerm = clamp(dot(normalWS, halfAngle), 0, 1);
    blinnTerm = lightAngleCos != 0 ? blinnTerm : 0;
    blinnTerm = pow(blinnTerm, smoothness);
    return blinnTerm;
}

vec4 getLight(vec3 posWS, vec3 normalWS, bool calcSpecular, float smoothness, vec3 cameraPosWS){
    vec4 light = vec4(0, 0, 0, 0);

    for (int i = 0; i < lightsCount; ++i)
    {
        vec3 lightDirWS = getLightDirWS(lights[i], posWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0, 1);
        float attenuationTerm = getAttenuationTerm(lights[i], posWS);
        light += lights[i].intensity * lightAngleCos * attenuationTerm;

        if (calcSpecular)
        {
            float specularTerm = getSpecularTerm(smoothness, lightDirWS, lightAngleCos, posWS, normalWS, cameraPosWS);
            light += lightAngleCos * specularTerm * attenuationTerm;
        }
    }

    return light;
}

vec4 doGammaCorrection(vec4 color){
    vec4 gamma = vec4(gammaCorrection, gammaCorrection, gammaCorrection, 1);
    return pow(color, gamma);
}

#endif //LIGHTING