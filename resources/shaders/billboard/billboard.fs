uniform sampler2D _Texture;

in vec2 uv;
out vec4 outColor;

void main(){
    vec4 texColor = texture(_Texture, uv);
    if (texColor.a < 0.1)
        discard;
    outColor = texColor;
}