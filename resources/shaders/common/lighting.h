#ifndef LIGHTING
#define LIGHTING

#include "graphics_backend_macros.h"
#include "shadows.h"

constant float PI = 3.14159265359;


/// Data ///

struct PointLight
{
    packed_float3 PositionWS;
    float Padding0;
    packed_float3 Intensity;
    float Attenuation;
};

struct SpotLight
{
    packed_float3 PositionWS;
    float Padding0;
    packed_float3 DirectionWS;
    float Attenuation;
    packed_float3 Intensity;
    float CutOffCos;
};

struct LightingStruct
{
    packed_float3 _AmbientLight;
    int _PointLightsCount;

    packed_float3 _DirLightDirectionWS;
    float _HasDirectionalLight;
    packed_float3 _DirLightIntensity;

    int _SpotLightsCount;

    PointLight _PointLights[MAX_POINT_LIGHT_SOURCES];
    SpotLight _SpotLights[MAX_SPOT_LIGHT_SOURCES];
};



/// Helpers ///

half3_type sampleReflection(CUBEMAP_HALF_PARAMETER(reflectionCube, reflectionCubeSampler), float cubeMips, float3 normalWS, float3 posWS, float roughness, float3 cameraPosWS)
{
    #if defined(_REFLECTION)
    float3 viewDirWS = normalize(posWS - cameraPosWS);
    float3 reflectedViewWS = reflect(viewDirWS, normalWS);
    return SAMPLE_TEXTURE_LOD(reflectionCube, reflectionCubeSampler, reflectedViewWS, roughness * cubeMips).rgb;
    #else
    return half3(0);
    #endif
}

float3 unpackNormal(float3 normalTS, float3 normalWS, float3 tangentWS, float normalIntensity)
{
    normalWS = normalize(normalWS);
    tangentWS = normalize(tangentWS);
    // Gramm-Schmidt process to ensure that tangent and normal are still orthogonal after interpolation
    tangentWS = normalize(tangentWS - dot(tangentWS, normalWS) * normalWS);
    float3 bitangentWS = cross(tangentWS, normalWS);

    normalTS = normalTS * 2.0 - 1.0;
    normalTS *= float3(normalIntensity, normalIntensity, 1);
    normalTS = normalize(normalTS);
    float3x3 TBN = float3x3(tangentWS, bitangentWS, normalWS);
    normalWS = normalize(TBN * normalTS);
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
    return float3(0);
    #endif
}

float3 getLightBlinnPhong(float3 posWS, float3 normalWS, float3 albedo, float4 specular, float roughness, float3 cameraPosWS, LightingStruct lightingData)
{
    float3 viewDirWS = normalize(cameraPosWS - posWS);
    float smoothness = 1 - roughness;

    float3 directLighting = float3(0);
    if (lightingData._HasDirectionalLight > 0){
        float3 lightDirWS = normalize(-lightingData._DirLightDirectionWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float shadowTerm = getDirLightShadowTerm(posWS, lightAngleCos);

        directLighting += albedo * lightingData._DirLightIntensity * lightAngleCos * shadowTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS);
    }

    for (int i = 0; i < lightingData._PointLightsCount; ++i)
    {
        float3 lightDirWS = normalize(lightingData._PointLights[i].PositionWS - posWS);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float dist = distance(lightingData._PointLights[i].PositionWS, posWS);
        float attenuationTerm = 1 / (1 + lightingData._PointLights[i].Attenuation * dist * dist);

        directLighting += albedo * lightingData._PointLights[i].Intensity * lightAngleCos * attenuationTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS) * attenuationTerm;
    }

    for (int i = 0; i < lightingData._SpotLightsCount; ++i){
        float3 lightDirWS = normalize(lightingData._SpotLights[i].PositionWS - posWS);
        float cutOffCos = clamp(dot(lightingData._SpotLights[i].DirectionWS, -lightDirWS), 0.0, 1.0);
        float lightAngleCos = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float dist = distance(lightingData._SpotLights[i].PositionWS, posWS);

        float attenuationTerm = 1 / (1 + lightingData._SpotLights[i].Attenuation * dist * dist);
        attenuationTerm *= clamp((cutOffCos - lightingData._SpotLights[i].CutOffCos) / (1 - lightingData._SpotLights[i].CutOffCos), 0.0, 1.0);
        attenuationTerm *= step(lightingData._SpotLights[i].CutOffCos, cutOffCos);

        float shadowTerm = getSpotLightShadowTerm(i, posWS, lightAngleCos);

        directLighting += albedo * lightingData._SpotLights[i].Intensity * lightAngleCos * attenuationTerm * shadowTerm;
        directLighting += getSpecularTermBlinnPhong(specular, smoothness, lightDirWS, viewDirWS, normalWS) * attenuationTerm;
    }

    return lightingData._AmbientLight * albedo + directLighting;
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

void getLightSourcePBR(float3 normalWS, float3 viewDirWS, float3 lightDirWS, float roughness, float3 F0, float metallness, OUT(float3) diffuse, OUT(float3) specular)
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
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - metallness;
    diffuse = kD / PI;
}

float3 getLightPBR(float3 posWS, float3 normalWS, float3 albedo, float roughness, float metallness, half3 reflectionIrradiance, float3 cameraPosWS, LightingStruct lightingData)
{
    float3 viewDirWS = normalize(cameraPosWS - posWS);
    float3 F0 = mix(float3(0.04), albedo, metallness);
    roughness =  clamp(roughness, 0.01, 1.0);

    float3 diffuse;
    float3 specular;
    float3 directLighting = float3(0);
    if (lightingData._HasDirectionalLight > 0){
        float3 lightDirWS = normalize(-lightingData._DirLightDirectionWS);
        
        float NdotL = max(dot(normalWS, lightDirWS), 0.0);
        float shadowTerm = getDirLightShadowTerm(posWS, NdotL);
        float3 radiance = lightingData._DirLightIntensity * NdotL * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }

    for (int i = 0; i < lightingData._PointLightsCount; ++i)
    {
        float3 lightDirWS = normalize(lightingData._PointLights[i].PositionWS - posWS);
        float NdotL = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float dist = distance(lightingData._PointLights[i].PositionWS, posWS);
        float attenuationTerm = 1 / (1 + lightingData._PointLights[i].Attenuation * dist * dist);

        float shadowTerm = getPointLightShadowTerm(i, posWS, NdotL);
        float3 radiance = lightingData._PointLights[i].Intensity * NdotL * attenuationTerm * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }
    
    for (int i = 0; i < lightingData._SpotLightsCount; ++i){
        float3 lightDirWS = normalize(lightingData._SpotLights[i].PositionWS - posWS);
        float cutOffCos = clamp(dot(lightingData._SpotLights[i].DirectionWS, -lightDirWS), 0.0, 1.0);
        float NdotL = clamp(dot(normalWS, lightDirWS), 0.0, 1.0);
        float dist = distance(lightingData._SpotLights[i].PositionWS, posWS);
    
        float attenuationTerm = 1 / (1 + lightingData._SpotLights[i].Attenuation * dist * dist);
        attenuationTerm *= clamp((cutOffCos - lightingData._SpotLights[i].CutOffCos) / (1 - lightingData._SpotLights[i].CutOffCos), 0.0, 1.0);
        attenuationTerm *= step(lightingData._SpotLights[i].CutOffCos, cutOffCos);
    
        float shadowTerm = getSpotLightShadowTerm(i, posWS, NdotL);
        float3 radiance = lightingData._SpotLights[i].Intensity * NdotL * attenuationTerm * shadowTerm;

        getLightSourcePBR(normalWS, viewDirWS, lightDirWS, roughness, F0, metallness, diffuse, specular);
        directLighting += (albedo * diffuse + specular) * radiance;
    }

    float3 indirectLighting = lightingData._AmbientLight;
    indirectLighting += F0 * float3(reflectionIrradiance);

    return albedo.rgb * indirectLighting + directLighting;
}

#endif //LIGHTING