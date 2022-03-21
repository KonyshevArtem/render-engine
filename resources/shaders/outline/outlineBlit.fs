in vec2 uv;

uniform sampler2D _Tex;
uniform sampler2D _OrigTex;
uniform vec4 _Color;

out vec4 outColor;

void main(){
    float alpha = texture(_Tex, uv).a;
    float origAlpha = texture(_OrigTex, uv).a;
    float a = max(alpha - origAlpha, 0);
    if (a < 0.01)
        discard;

    outColor = _Color;
}