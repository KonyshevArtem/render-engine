#version 410
#include "../lighting.glsl"
#include "../camera_data.glsl"

smooth in vec4 positionWS;
smooth in vec3 normalWS;
smooth in vec4 color;
smooth in vec2 uv;

out vec4 outColor;

uniform sampler2D albedo;
uniform float smoothness;

void main(){
    vec4 albedoColor = texture(albedo, uv) * color;
    outColor = albedoColor * ambientLightColor;
    outColor += albedoColor * getLight(positionWS.xyz, normalize(normalWS), true, smoothness, cameraPosWS);
    outColor = doGammaCorrection(outColor);
}