in vec2[5] uvs;

#include "outlineBlit_per_material.cg"

uniform sampler2D _Tex;

out vec4 outColor;

void main(){
    float a0 = texture(_Tex, uvs[0]).a;
    float a1 = texture(_Tex, uvs[1]).a;
    float a2 = texture(_Tex, uvs[2]).a;
    float a3 = texture(_Tex, uvs[3]).a;
    float maxAlpha = max(a0, max(a1, max(a2, a3)));
    maxAlpha = step(0.01, maxAlpha);

    float alpha = step(0.01, texture(_Tex, uvs[4]).a);
    float a = max(maxAlpha - alpha, 0);
    if (a < 0.01)
        discard;

    outColor = _Color;
}