#version 410

smooth in vec4 color;

uniform float phase;

out vec4 outColor;

vec4 white = vec4(1, 1, 1, 1);

void main(){
    outColor = mix(color, white, sin(phase * 3.14));
}