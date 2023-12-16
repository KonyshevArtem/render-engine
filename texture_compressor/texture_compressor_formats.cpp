#include "texture_compressor_formats.h"
#ifdef TEXTURE_COMPRESSOR_WINDOWS
#include <GL/glew.h>
#elif TEXTURE_COMPRESSOR_MACOS
#include <OpenGL/glu.h>
#endif

#include <vector>
#include <unordered_map>

#include "lodepng.h"

namespace TextureCompressorFormats
{
    std::vector<std::pair<int, std::string>> inputFormats =
            {
                    {LCT_RGB,        "RGB"},
                    {LCT_RGBA,       "RGBA"},
                    {LCT_GREY,       "GREY"},
                    {LCT_GREY_ALPHA, "GREY ALPHA"},
            };

    std::unordered_map<int, int> colorTypeToFormat =
            {
                    {LCT_RGB,        GL_RGB},
                    {LCT_RGBA,       GL_RGBA},
                    {LCT_GREY,       GL_RED},
                    {LCT_GREY_ALPHA, GL_RG}
            };

    std::vector<std::pair<int, std::string>> compressedFormats =
            {
                    {GL_RGB, "RGB"},
                    {GL_RGBA, "RGBA"},
                    {GL_SRGB, "sRGB"},
                    {GL_SRGB_ALPHA, "sRGBA"},
                    {GL_COMPRESSED_RGB, "Compressed RGB"},
                    {GL_COMPRESSED_RGBA, "Compressed RGBA"},
                    {GL_COMPRESSED_SRGB, "Compressed sRGB"},
                    {GL_COMPRESSED_SRGB_ALPHA, "Compressed sRGBA"},

#if GL_ARB_texture_rg
                    {GL_COMPRESSED_RED, "Compressed Red"},
                    {GL_COMPRESSED_RG, "Compressed RG"},
#endif

                    {GL_R3_G3_B2, "R3 G3 B2"},
                    {GL_RGB4, "RGB4"},
                    {GL_RGB5, "RGB5"},
                    {GL_RGB8, "RGB8"},
                    {GL_RGB10, "RGB10"},
                    {GL_RGB12, "RGB12"},
                    {GL_RGB16, "RGB16"},
                    {GL_RGBA2, "RGBA2"},
                    {GL_RGBA4, "RGBA4"},
                    {GL_RGB5_A1, "RGB5 A1"},
                    {GL_RGBA8, "RGBA8"},
                    {GL_RGB10_A2, "RGB10 A2"},
                    {GL_RGBA12, "RGBA12"},
                    {GL_RGBA16, "RGBA16"},

#if GL_EXT_texture_compression_s3tc
                    {GL_COMPRESSED_RGB_S3TC_DXT1_EXT, "RGB S3TC DXT1"},
                    {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, "RGBA S3TC DXT1"},
                    {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, "RGBA S3TC DXT3"},
                    {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, "RGBA S3TC DXT5"},
#endif

#if GL_EXT_texture_sRGB
                    {GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, "SRGB S3TC DXT1"},
                    {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, "sRGBA S3TC DXT1"},
                    {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, "sRGBA S3TC DXT3"},
                    {GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, "sRGBA S3TC DXT5"},
#endif

#if GL_ARB_texture_compression_rgtc
                    {GL_COMPRESSED_RED_RGTC1, "RED RGTC1"},
                    {GL_COMPRESSED_SIGNED_RED_RGTC1, "SIGNED RED RGTC1"},
                    {GL_COMPRESSED_RG_RGTC2, "RG RGTC2"},
                    {GL_COMPRESSED_SIGNED_RG_RGTC2, "SIGNED RG RGTC2"},
#endif
            };

    int GetFormatByColorType(int colorType)
    {
        return colorTypeToFormat[colorType];
    }

    std::string GetCompressedFormatName(int format)
    {
        for (const auto &pair: compressedFormats)
        {
            if (pair.first == format)
            {
                return pair.second;
            }
        }

        return "Unknown";
    }

    const std::vector<std::pair<int, std::string>> &GetInputFormats()
    {
        return inputFormats;
    }

    const std::vector<std::pair<int, std::string>> &GetCompressedFormats()
    {
        return compressedFormats;
    }
}