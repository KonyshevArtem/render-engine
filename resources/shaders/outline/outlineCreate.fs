in vec2[4] uvs;

uniform sampler2D _Tex;

out vec4 outColor;

void main(){
    float a0 = texture(_Tex, uvs[0]).a;
    float a1 = texture(_Tex, uvs[1]).a;
    float a2 = texture(_Tex, uvs[2]).a;
    float a3 = texture(_Tex, uvs[3]).a;
    float maxAlpha = max(a0, max(a1, max(a2, a3)));
    outColor = vec4(1, 1, 1, maxAlpha);
}