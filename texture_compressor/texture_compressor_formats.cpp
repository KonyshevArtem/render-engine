#include "texture_compressor_formats.h"

#include <vector>
#include <unordered_map>

#include "lodepng.h"

namespace TextureCompressorFormats
{
    std::vector<TextureTypeInfo> textureTypesInfo =
            {
                    {"Texture 2D", TextureType::TEXTURE_2D, 1},
                    {"Cubemap", TextureType::TEXTURE_CUBEMAP, 6},
            };

    std::vector<std::pair<int, std::string>> inputFormats =
            {
                    {LCT_RGB,        "RGB"},
                    {LCT_RGBA,       "RGBA"},
                    {LCT_GREY,       "GREY"},
                    {LCT_GREY_ALPHA, "GREY ALPHA"},
            };

    std::unordered_map<int, TexturePixelFormat> colorTypeToFormat =
            {
                    {LCT_RGB,        TexturePixelFormat::RGB},
                    {LCT_RGBA,       TexturePixelFormat::RGBA},
                    {LCT_GREY,       TexturePixelFormat::RED},
                    {LCT_GREY_ALPHA, TexturePixelFormat::RG}
            };

    std::vector<std::pair<TextureInternalFormat, std::string>> textureFormats =
            {
                    {TextureInternalFormat::RED, "Red"},
                    {TextureInternalFormat::RG, "RG"},
                    {TextureInternalFormat::RGB, "RGB"},
                    {TextureInternalFormat::RGBA, "RGBA"},
                    {TextureInternalFormat::SRGB, "sRGB"},
                    {TextureInternalFormat::SRGB_ALPHA, "sRGBA"},
                    {TextureInternalFormat::COMPRESSED_RGB, "Compressed RGB"},
                    {TextureInternalFormat::COMPRESSED_RGBA, "Compressed RGBA"},
                    {TextureInternalFormat::COMPRESSED_SRGB, "Compressed sRGB"},
                    {TextureInternalFormat::COMPRESSED_SRGB_ALPHA, "Compressed sRGBA"},

                    {TextureInternalFormat::COMPRESSED_RED, "Compressed Red"},
                    {TextureInternalFormat::COMPRESSED_RG, "Compressed RG"},

                    {TextureInternalFormat::R3_G3_B2, "R3 G3 B2"},
                    {TextureInternalFormat::RGB4, "RGB4"},
                    {TextureInternalFormat::RGB5, "RGB5"},
                    {TextureInternalFormat::RGB8, "RGB8"},
                    {TextureInternalFormat::RGB10, "RGB10"},
                    {TextureInternalFormat::RGB12, "RGB12"},
                    {TextureInternalFormat::RGB16, "RGB16"},
                    {TextureInternalFormat::RGBA2, "RGBA2"},
                    {TextureInternalFormat::RGBA4, "RGBA4"},
                    {TextureInternalFormat::RGB5_A1, "RGB5 A1"},
                    {TextureInternalFormat::RGBA8, "RGBA8"},
                    {TextureInternalFormat::RGB10_A2, "RGB10 A2"},
                    {TextureInternalFormat::RGBA12, "RGBA12"},
                    {TextureInternalFormat::RGBA16, "RGBA16"},

                    {TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT, "RGB S3TC DXT1"},
                    {TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT, "RGBA S3TC DXT1"},
                    {TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT, "RGBA S3TC DXT3"},
                    {TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT, "RGBA S3TC DXT5"},

                    {TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT, "SRGB S3TC DXT1"},
                    {TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, "sRGBA S3TC DXT1"},
                    {TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, "sRGBA S3TC DXT3"},
                    {TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, "sRGBA S3TC DXT5"},

                    {TextureInternalFormat::COMPRESSED_RED_RGTC1, "RED RGTC1"},
                    {TextureInternalFormat::COMPRESSED_SIGNED_RED_RGTC1, "SIGNED RED RGTC1"},
                    {TextureInternalFormat::COMPRESSED_RG_RGTC2, "RG RGTC2"},
                    {TextureInternalFormat::COMPRESSED_SIGNED_RG_RGTC2, "SIGNED RG RGTC2"},
            };

    TexturePixelFormat GetPixelFormatByColorType(int colorType)
    {
        return colorTypeToFormat[colorType];
    }

    std::string GetTextureFormatName(TextureInternalFormat format)
    {
        for (const auto &pair: textureFormats)
        {
            if (pair.first == format)
            {
                return pair.second;
            }
        }

        return "Unknown";
    }

    const TextureTypeInfo &GetTextureTypeInfo(TextureType textureType)
    {
        const static TextureTypeInfo invalid{.Count = -1};

        for (const auto &typeInfo: textureTypesInfo)
        {
            if (typeInfo.Type == textureType)
            {
                return typeInfo;
            }
        }

        return invalid;
    }

    const std::vector<TextureTypeInfo> &GetTextureTypesInfo()
    {
        return textureTypesInfo;
    }

    const std::vector<std::pair<int, std::string>> &GetInputFormats()
    {
        return inputFormats;
    }

    const std::vector<std::pair<TextureInternalFormat, std::string>> &GetTextureFormats()
    {
        return textureFormats;
    }
}