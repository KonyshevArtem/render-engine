#version 410

smooth in vec4 color;

uniform float time;
uniform float loopDuration;

out vec4 outColor;

vec4 white = vec4(1, 1, 1, 1);

void main(){
    float phase = sin(mod(time, loopDuration) / loopDuration * 3.14);
    outColor = mix(color, white, phase);
}