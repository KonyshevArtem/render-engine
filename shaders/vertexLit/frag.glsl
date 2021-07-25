#version 410

smooth in vec4 color;
smooth in vec2 uv;

uniform sampler2D albedo;

out vec4 outColor;

void main(){
    outColor = texture(albedo, uv) * color;
}