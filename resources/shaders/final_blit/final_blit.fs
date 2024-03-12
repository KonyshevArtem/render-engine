in vec2 uv;

uniform sampler2D _BlitTexture;

out vec4 outColor;

float gamma = 2.2;

vec3 GammaCorrection(vec3 color)
{
    return pow(color, vec3(1 / gamma));
}

vec3 ReinhardTonemapping(vec3 color)
{
    color = color / (color + vec3(1.0));
    return GammaCorrection(color);
}

void main()
{
    vec3 color = texture(_BlitTexture, uv).rgb;
    outColor = vec4(ReinhardTonemapping(color), 1);
}