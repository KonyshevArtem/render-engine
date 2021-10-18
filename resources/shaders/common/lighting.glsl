#ifndef LIGHTING
#define LIGHTING

#include "shadows.glsl"

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

    PointLight _PointLights[MAX_POINT_LIGHT_SOURCES];
    int _PointLightsCount;

    SpotLight _SpotLights[MAX_SPOT_LIGHT_SOURCES];
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

vec3 getNormalWS(Varyings varyings)
{
    vec3 normalWS = normalize(varyings.NormalWS);
    #ifdef _NORMAL_MAP
    vec3 tangentWS = normalize(varyings.TangentWS);
    // Gramm-Schmidt process to ensure that tangent and normal are still orthogonal after interpolation
    tangentWS = normalize(tangentWS - dot(tangentWS, normalWS) * normalWS);
    vec3 bitangentWS = cross(tangentWS, normalWS);
    vec3 normalTS = texture(_NormalMap, varyings.UV * _NormalMapST.zw + _NormalMapST.xy).rgb;
    normalTS = normalize(normalTS * 2.0 - 1.0);
    mat3 TBN = mat3(tangentWS, bitangentWS, normalWS);
    normalWS = normalize(TBN * normalTS);
    #endif
    return normalWS;
}

vec4 getLight(Varyings varyings){
    vec4 light = vec4(0, 0, 0, 0);

    vec3 posWS = vec3(varyings.PositionWS);
    vec3 normalWS = getNormalWS(varyings);

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
            float shadowTerm = getSpotLightShadowTerm(varyings, i, lightAngleCos);
            light += (_SpotLights[i].Intensity * lightAngleCos * attenuationTerm + specularTerm * attenuationTerm) * shadowTerm;
        }
    }

    return vec4((light + _AmbientLight).xyz, 1);
}

#endif //LIGHTING