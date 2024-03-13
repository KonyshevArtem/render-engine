#define NO_TONEMAPPING          0
#define REINHARD_TONEMAPPING    1

in vec2 uv;

uniform sampler2D _BlitTexture;

out vec4 outColor;

layout(std140) uniform PerMaterialData
{
    float _Gamma;
    uint _TonemappingMode;
};

vec3 GammaCorrection(vec3 color)
{
    return pow(color, vec3(1 / _Gamma));
}

vec3 ReinhardTonemapping(vec3 color)
{
    color = color / (color + vec3(1.0));
    return GammaCorrection(color);
}

void main()
{
    vec3 color = texture(_BlitTexture, uv).rgb;

    switch (_TonemappingMode)
    {
        case NO_TONEMAPPING:
            color = GammaCorrection(color);
            break;
        case REINHARD_TONEMAPPING:
            color = ReinhardTonemapping(color);
            break;
    }

    outColor = vec4(color, 1);
}