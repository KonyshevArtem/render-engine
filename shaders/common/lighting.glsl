#ifndef LIGHTING
#define LIGHTING

layout(std140) struct DirectionalLight
{
    vec3 DirectionWS;
    vec4 Intensity;
};

layout(std140) struct PointLight
{
    vec3 PositionWS;
    vec4 Intensity;
    float Attenuation;
};

layout(std140) struct SpotLight
{
    vec3 PositionWS;
    vec3 DirectionWS;
    vec4 Intensity;
    float Attenuation;
    float CutOffCos;
};

layout(std140) uniform Lighting
{
    vec4 _AmbientLight;

    DirectionalLight _DirectionalLight;
    bool _HasDirectionalLight;

    PointLight _PointLights[MAX_LIGHT_SOURCES];
    int _PointLightsCount;

    SpotLight _SpotLights[MAX_LIGHT_SOURCES];
    int _SpotLightsCount;
};

float getSpecularTerm(vec3 lightDirWS, float lightAngleCos, vec3 posWS, vec3 normalWS){
    #ifdef _SMOOTHNESS
    vec3 viewDirWS = normalize(_CameraPosWS - posWS);
    vec3 halfAngle = normalize(lightDirWS + viewDirWS);
    float blinnTerm = clamp(dot(normalWS, halfAngle), 0, 1);
    blinnTerm = lightAngleCos != 0 ? blinnTerm : 0;
    blinnTerm = pow(blinnTerm, _Smoothness);
    return blinnTerm;
    #else
    return 0;
    #endif
}

vec4 getLight(vec3 posWS, vec3 normalWS){
    vec4 light = vec4(0, 0, 0, 0);

    if (_HasDirectionalLight){
        vec3 lightDirWS = normalize(-_DirectionalLight.DirectionWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0, 1);
        float specularTerm = getSpecularTerm(lightDirWS, lightAngleCos, posWS, normalWS);
        light += _DirectionalLight.Intensity * lightAngleCos + specularTerm;
    }

    for (int i = 0; i < _PointLightsCount; ++i)
    {
        vec3 lightDirWS = normalize(_PointLights[i].PositionWS - posWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0, 1);
        float distance = distance(_PointLights[i].PositionWS, posWS);
        float attenuationTerm = 1 / (1 + _PointLights[i].Attenuation * distance * distance);
        float specularTerm = getSpecularTerm(lightDirWS, lightAngleCos, posWS, normalWS);
        light += _PointLights[i].Intensity * lightAngleCos * attenuationTerm + specularTerm * attenuationTerm;
    }

    for (int i = 0; i < _SpotLightsCount; ++i){
        vec3 lightDirWS = normalize(_SpotLights[i].PositionWS - posWS);
        float cutOffCos = clamp(dot(_SpotLights[i].DirectionWS, -lightDirWS), 0, 1);
        if (cutOffCos > _SpotLights[i].CutOffCos)
        {
            float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0, 1);
            float distance = distance(_SpotLights[i].PositionWS, posWS);
            float attenuationTerm = 1 / (1 + _SpotLights[i].Attenuation * distance * distance);
            attenuationTerm *= clamp((cutOffCos - _SpotLights[i].CutOffCos) / (1 - _SpotLights[i].CutOffCos), 0, 1);
            float specularTerm = getSpecularTerm(lightDirWS, lightAngleCos, posWS, normalWS);
            light += _SpotLights[i].Intensity * lightAngleCos * attenuationTerm + specularTerm * attenuationTerm;
        }
    }

    return light;
}

    #endif//LIGHTING