#ifndef LIGHTING
#define LIGHTING

#include "global_defines.h"
#include "shadows.h"


/// Data ///

struct PointLight
{
    float3 PositionWS;
    float Padding0;
    float3 Intensity;
    float Range;
};

struct SpotLight
{
    float3 PositionWS;
    float Padding0;
    float3 DirectionWS;
    float Range;
    float3 Intensity;
    float CutOffCos;
};

cbuffer Lighting : register(b2)
{
    float3 _AmbientLight;
    int _PointLightsCount;

    float3 _DirLightDirectionWS;
    float _HasDirectionalLight;
    float3 _DirLightIntensity;

    int _SpotLightsCount;

    PointLight _PointLights[MAX_POINT_LIGHT_SOURCES];
    SpotLight _SpotLights[MAX_SPOT_LIGHT_SOURCES];

    float3 _Padding0;
    float _ReflectionCubeMips;
};

TextureCube _ReflectionCube : register(t3);
SamplerState sampler_ReflectionCube : register(s3);

/// Helpers ///

float lightAttenuation(float distance, float range)
{
    // https://www.desmos.com/calculator/scz7zhonfw
    float t = 1 - distance / range;
    const float p1 = 0;
    const float p2 = 0;
    const float p3 = 1;

    float a = p2 + (1 - t) * (p1 - p2);
    float b = p2 + t * (p3 - p2);
    return a + t * (b - a);
}

half3 sampleReflection(float3 normalWS, float3 posWS, float roughness, float3 cameraPosWS)
{
    #if defined(_REFLECTION)
    float3 viewDirWS = normalize(posWS - cameraPosWS);
    float3 reflectedViewWS = reflect(viewDirWS, normalWS);
    return _ReflectionCube.SampleLevel(sampler_ReflectionCube, reflectedViewWS, roughness * _ReflectionCubeMips).rgb;
    #else
    return (half3) 0.0h;
    #endif
}

float3 unpackNormal(float2 normalTS, float3 normalWS, float3 tangentWS)
{
    normalWS = normalize(normalWS);
    tangentWS = normalize(tangentWS);
    // Gramm-Schmidt process to ensure that tangent and normal are still orthogonal after interpolation
    tangentWS = normalize(tangentWS - dot(tangentWS, normalWS) * normalWS);
    float3 bitangentWS = cross(normalWS, tangentWS);

    normalTS = normalTS * 2.0 - 1.0;
    float3 normal = float3(normalTS.xy, 0);
    normal.z = sqrt(1 - normalTS.x * normalTS.x - normalTS.y * normalTS.y);
    normal = normalize(normal);

    float3x3 TBN = float3x3(tangentWS.x, bitangentWS.x, normalWS.x,
                            tangentWS.y, bitangentWS.y, normalWS.y,
                            tangentWS.z, bitangentWS.z, normalWS.z);
    normalWS = normalize(mul(TBN, normal));
    return normalWS;
}


/// Blinn-Phong ///


float3 getSpecularTermBlinnPhong(float4 specular, float smoothness, float3 lightDirWS, float3 viewDirWS, float3 normalWS){
    #if defined(_SPECULAR)
    float3 halfAngle = normalize(lightDirWS + viewDirWS);
    float blinnTerm = clamp(dot(normalWS, halfAngle), 0.0, 1.0);
    blinnTerm = pow(blinnTerm, smoothness) * specular.a;
    return specular.rgb * max(blinnTerm, 0.0);
    #else
    return (float3) 0.0;
    #endif
}

float3 getLightBlinnPhong(float3 posWS, float3 normalWS, float3 albedo, float4 specular, float roughness, float3 cameraPosWS)
{
    float3 fragToCamera = cameraPosWS - posWS;
    float fragDistance = length(fragToCamera);
    float3 viewDirWS = fragToCamera / fragDistance;
    float smoothness = 1 - roughness;

    float3 directLighting = (float3) 0.0;
    if (_HasDirectionalLight > 0)
    {
        float3 lightDirWS = normalize(-_DirLightDirectionWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float shadowTerm = getDirLightShadowTerm(posWS, lightAngleCos, fragDistance);

        directLighting += albedo * _DirLightIntensity * lightAngleCos * shadowTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS);
    }

    for (int i = 0; i < _PointLightsCount; ++i)
    {
        float dist = distance(_PointLights[i].PositionWS, posWS);
        if (dist > _PointLights[i].Range)
            continue;

        float3 lightDirWS = normalize(_PointLights[i].PositionWS - posWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float attenuationTerm = saturate(lightAttenuation(dist, _PointLights[i].Range));
        float shadowTerm = getPointLightShadowTerm(i, posWS, lightAngleCos);

        directLighting += albedo * _PointLights[i].Intensity * lightAngleCos * attenuationTerm * shadowTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS) * attenuationTerm;
    }

    for (int i = 0; i < _SpotLightsCount; ++i)
    {
        float dist = distance(_SpotLights[i].PositionWS, posWS);
        if (dist > _SpotLights[i].Range)
            continue;

        float3 lightDirWS = normalize(_SpotLights[i].PositionWS - posWS);
        float cutOffCos = clamp(dot(_SpotLights[i].DirectionWS, -lightDirWS), 0.0, 1.0);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);

        float attenuationTerm = saturate(lightAttenuation(dist, _SpotLights[i].Range));
        attenuationTerm *= clamp((cutOffCos - _SpotLights[i].CutOffCos) / (1 - _SpotLights[i].CutOffCos), 0.0, 1.0);
        attenuationTerm *= step(_SpotLights[i].CutOffCos, cutOffCos);

        float shadowTerm = getSpotLightShadowTerm(i, posWS, lightAngleCos);

        directLighting += albedo * _SpotLights[i].Intensity * lightAngleCos * attenuationTerm * shadowTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS) * attenuationTerm;
    }

    return _AmbientLight * albedo + directLighting;
}



/// PBR ///


float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a2 = roughness*roughness;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void getLightSourcePBR(float3 normalWS, float3 viewDirWS, float3 lightDirWS, float roughness, float3 F0, float metallness, out float3 diffuse, out float3 specular)
{
    float3 halfVector = normalize(lightDirWS + viewDirWS);

    float NdotV = max(dot(normalWS, viewDirWS), 0.0);
    float NdotL = max(dot(normalWS, lightDirWS), 0.0);
    
    float NDF = DistributionGGX(normalWS, halfVector, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 F = fresnelSchlick(max(dot(halfVector, viewDirWS), 0.0), F0);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    specular = numerator / max(denominator, 0.001);
    
    float3 kS = F;
    float3 kD = ((float3) 1.0) - kS;
    kD *= 1.0 - metallness;
    diffuse = kD / PI;
}

float3 getLightPBR(float3 posWS, float3 normalWS, float3 albedo, float roughness, float metallness, half3 reflectionIrradiance, float3 cameraPosWS)
{
    float3 fragToCamera = cameraPosWS - posWS;
    float fragDistance = length(fragToCamera);
    float3 viewDirWS = fragToCamera / fragDistance;
    float3 F0 = lerp((float3) 0.04, albedo, metallness);
    roughness =  clamp(roughness, 0.01, 1.0);

    float3 diffuse;
    float3 specular;
    float3 directLighting = (float3) 0;
    if (_HasDirectionalLight > 0)
    {
        float3 lightDirWS = normalize(-_DirLightDirectionWS);
        
        float NdotL = max(dot(normalWS, lightDirWS), 0.0);
        float shadowTerm = getDirLightShadowTerm(posWS, NdotL, fragDistance);
        float3 radiance = _DirLightIntensity * NdotL * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }

    for (int i = 0; i < _PointLightsCount; ++i)
    {
        float dist = distance(_PointLights[i].PositionWS, posWS);
        if (dist > _PointLights[i].Range)
            continue;

        float3 lightDirWS = normalize(_PointLights[i].PositionWS - posWS);
        float NdotL = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float attenuationTerm = saturate(lightAttenuation(dist, _PointLights[i].Range));
        float shadowTerm = getPointLightShadowTerm(i, posWS, NdotL);
        float3 radiance = _PointLights[i].Intensity * NdotL * attenuationTerm * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }
    
    for (int i = 0; i < _SpotLightsCount; ++i)
    {
        float dist = distance(_SpotLights[i].PositionWS, posWS);
        if (dist > _SpotLights[i].Range)
            continue;

        float3 lightDirWS = normalize(_SpotLights[i].PositionWS - posWS);
        float cutOffCos = clamp(dot(_SpotLights[i].DirectionWS, -lightDirWS), 0.0, 1.0);
        float NdotL = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);

        float attenuationTerm = saturate(lightAttenuation(dist, _SpotLights[i].Range));
        attenuationTerm *= clamp((cutOffCos - _SpotLights[i].CutOffCos) / (1 - _SpotLights[i].CutOffCos), 0.0, 1.0);
        attenuationTerm *= step(_SpotLights[i].CutOffCos, cutOffCos);
        float shadowTerm = getSpotLightShadowTerm(i, posWS, NdotL);
        float3 radiance = _SpotLights[i].Intensity * NdotL * attenuationTerm * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }

    float3 indirectLighting = _AmbientLight;
    indirectLighting += F0 * float3(reflectionIrradiance);

    return albedo.rgb * indirectLighting + directLighting;
}

#endif //LIGHTING