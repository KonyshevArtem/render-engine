smooth in vec4 color;
smooth in vec2 uv;

uniform sampler2D _Albedo;

out vec4 outColor;

void main(){
    outColor = texture(_Albedo, uv) * color;
}