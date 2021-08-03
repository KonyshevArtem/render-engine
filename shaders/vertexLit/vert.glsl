#version 410
#include "../attributes.glsl"
#include "../lighting.glsl"

uniform vec4 albedoST;

smooth out vec4 color;
smooth out vec2 uv;

void main(){
    vec4 vertPositionWS = modelMatrix * vertPositionOS;
    gl_Position = projMatrix * viewMatrix * vertPositionWS;

    vec3 normalWS = normalize((modelNormalMatrix * vec4(vertNormalOS, 0)).xyz);
    color = vertColor * ambientLightColor;
    color += vertColor * getLight(vertPositionWS.xyz, normalWS, false, 0, vec3(0, 0, 0));
    color = doGammaCorrection(color);

    uv = texCoord * albedoST.zw + albedoST.xy;
}