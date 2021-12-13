uniform sampler2D _Texture;

in vec2 uv;
out vec4 outColor;

void main(){
    outColor = texture(_Texture, uv);
}