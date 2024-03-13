#define NONE                                    0
#define REINHARD_SIMPLE                         1
#define REINHARD_LUMINANCE                      2
#define REINHARD_LUMINANCE_WHITE_PRESERVING     3
#define ROMBINDAHOUSE                           4
#define FILMIC                                  5
#define UNCHARTED2                              6
#define ACES                                    7
#define ACES_APPROXIMATE                        8

in vec2 uv;

uniform sampler2D _BlitTexture;

out vec4 outColor;

layout(std140) uniform PerMaterialData
{
    float _OneOverGamma;
    float _Exposure;
    uint _TonemappingMode;
};

vec3 GammaCorrection(vec3 color)
{
    return pow(color, vec3(_OneOverGamma));
}

float GetLuminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 ReinhardTonemapping(vec3 color)
{
    color = color * _Exposure / (color / _Exposure + vec3(1.0));
    return GammaCorrection(color);
}

vec3 LumaBasedReinhardToneMapping(vec3 color)
{
    float luma = GetLuminance(color);
    float toneMappedLuma = luma / (1.0 + luma);
    color *= toneMappedLuma / luma;
    return GammaCorrection(color);
}

vec3 WhitePreservingLumaBasedReinhardToneMapping(vec3 color)
{
    float white = 2.0;
    float luma = GetLuminance(color);
    float toneMappedLuma = luma * (1.0 + luma / (white * white)) / (1.0 + luma);
    color *= toneMappedLuma / luma;
    return GammaCorrection(color);
}

vec3 RomBinDaHouseToneMapping(vec3 color)
{
    color = exp(-1.0 / (2.72 * color + 0.15));
    return GammaCorrection(color);
}

vec3 FilmicToneMapping(vec3 color)
{
    color = max(vec3(0.0), color - vec3(0.004));
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

vec3 Uncharted2ToneMapping(vec3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    color *= _Exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    return GammaCorrection(color);
}

vec3 ACESTonemapping(vec3 color){
    mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
    );
    mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602
    );
    vec3 v = m1 * color;
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    color = clamp(m2 * (a / b), 0.0, 1.0);
    return GammaCorrection(color);
}

vec3 ACESApproximateTonemapping(vec3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    color = clamp((color * (a * color + b)) / (color * (c * color + d ) + e), 0.0, 1.0);
    return GammaCorrection(color);
}

void main()
{
    vec3 color = texture(_BlitTexture, uv).rgb;

    // https://www.shadertoy.com/view/lslGzl
    // https://www.shadertoy.com/view/XsGfWV
    // https://www.shadertoy.com/view/tdffDl

    switch (_TonemappingMode)
    {
        case NONE:
            color = GammaCorrection(color);
            break;
        case REINHARD_SIMPLE:
            color = ReinhardTonemapping(color);
            break;
        case REINHARD_LUMINANCE:
            color = LumaBasedReinhardToneMapping(color);
            break;
        case REINHARD_LUMINANCE_WHITE_PRESERVING:
            color = WhitePreservingLumaBasedReinhardToneMapping(color);
            break;
        case ROMBINDAHOUSE:
            color = RomBinDaHouseToneMapping(color);
            break;
        case FILMIC:
            color = FilmicToneMapping(color);
            break;
        case UNCHARTED2:
            color = Uncharted2ToneMapping(color);
            break;
        case ACES:
            color = ACESTonemapping(color);
            break;
        case ACES_APPROXIMATE:
            color = ACESApproximateTonemapping(color);
            break;
    }

    outColor = vec4(color, 1);
}