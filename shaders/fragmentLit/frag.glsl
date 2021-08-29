#include "../lighting.glsl"
#include "../camera_data.glsl"

smooth in vec4 positionWS;
smooth in vec3 normalWS;
smooth in vec4 color;
smooth in vec2 uv;

out vec4 outColor;

uniform sampler2D _Albedo;
uniform float _Smoothness;

void main(){
    vec4 albedoColor = texture(_Albedo, uv) * color;
    outColor = albedoColor * _AmbientLightColor;
    outColor += albedoColor * getLight(positionWS.xyz, normalize(normalWS), true, _Smoothness, _CameraPosWS);
}