#ifndef LIGHTING
#define LIGHTING

layout(std140) struct LightData
{
    vec3 posOrDirWS;
    vec4 intensity;
    bool isDirectional;
    float attenuation;
};

layout(std140) uniform Lighting
{
    LightData _Lights[MAX_LIGHT_SOURCES];
    vec4 _AmbientLightColor;
    int _LightsCount;
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

    for (int i = 0; i < _LightsCount; ++i)
    {
        vec3 lightDirWS = getLightDirWS(_Lights[i], posWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0, 1);
        float attenuationTerm = getAttenuationTerm(_Lights[i], posWS);
        light += _Lights[i].intensity * lightAngleCos * attenuationTerm;

        if (calcSpecular)
        {
            float specularTerm = getSpecularTerm(smoothness, lightDirWS, lightAngleCos, posWS, normalWS, cameraPosWS);
            light += specularTerm * attenuationTerm;
        }
    }

    return light;
}

    #endif//LIGHTING