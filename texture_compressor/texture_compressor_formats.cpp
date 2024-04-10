#include "texture_compressor_formats.h"

#include <vector>

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

                    {TextureInternalFormat::R8, "R8"},
                    {TextureInternalFormat::R8_SNORM, "R8 SNORM"},
                    {TextureInternalFormat::R16, "R16"},
                    {TextureInternalFormat::R16_SNORM, "R16 SNORM"},
                    {TextureInternalFormat::RG8, "RG8"},
                    {TextureInternalFormat::RG8_SNORM, "RG8 SNORM"},
                    {TextureInternalFormat::RG16, "RG16"},
                    {TextureInternalFormat::RG16_SNORM, "RG16 SNORM"},
                    {TextureInternalFormat::R3_G3_B2, "R3 G3 B2"},
                    {TextureInternalFormat::RGB4, "RGB4"},
                    {TextureInternalFormat::RGB5, "RGB5"},
                    {TextureInternalFormat::RGB8, "RGB8"},
                    {TextureInternalFormat::RGB8_SNORM, "RGB8 SNORM"},
                    {TextureInternalFormat::RGB10, "RGB10"},
                    {TextureInternalFormat::RGB12, "RGB12"},
                    {TextureInternalFormat::RGB16, "RGB16"},
                    {TextureInternalFormat::RGBA2, "RGBA2"},
                    {TextureInternalFormat::RGBA4, "RGBA4"},
                    {TextureInternalFormat::RGB5_A1, "RGB5 A1"},
                    {TextureInternalFormat::RGBA8, "RGBA8"},
                    {TextureInternalFormat::RGBA8_SNORM, "RGBA8_SNORM"},
                    {TextureInternalFormat::RGB10_A2, "RGB10 A2"},
                    {TextureInternalFormat::RGB10_A2UI, "RGB10 A2UI"},
                    {TextureInternalFormat::RGBA12, "RGBA12"},
                    {TextureInternalFormat::RGBA16, "RGBA16"},
                    {TextureInternalFormat::SRGB8, "SRGB8"},
                    {TextureInternalFormat::SRGB8_ALPHA8, "SRGB8 ALPHA8"},
                    {TextureInternalFormat::R16F, "R16F"},
                    {TextureInternalFormat::RG16F, "RG16F"},
                    {TextureInternalFormat::RGB16F, "RGB16F"},
                    {TextureInternalFormat::RGBA16F, "RGBA16F"},
                    {TextureInternalFormat::R32F, "R32F"},
                    {TextureInternalFormat::RG32F, "RG32F"},
                    {TextureInternalFormat::RGB32F, "RGB32F"},
                    {TextureInternalFormat::RGBA32F, "RGBA32F"},
                    {TextureInternalFormat::R11F_G11F_B10F, "R11F G11F B10F"},
                    {TextureInternalFormat::RGB9_E5, "RGB9_E5"},
                    {TextureInternalFormat::R8I, "R8I"},
                    {TextureInternalFormat::R8UI, "R8UI"},
                    {TextureInternalFormat::R16I, "R16I"},
                    {TextureInternalFormat::R16UI, "R16UI"},
                    {TextureInternalFormat::R32I, "R32I"},
                    {TextureInternalFormat::R32UI, "R32UI"},
                    {TextureInternalFormat::RG8I, "RG8I"},
                    {TextureInternalFormat::RG8UI, "RG8UI"},
                    {TextureInternalFormat::RG16I, "RG16I"},
                    {TextureInternalFormat::RG16UI, "RG16UI"},
                    {TextureInternalFormat::RG32I, "RG32I"},
                    {TextureInternalFormat::RG32UI, "RG32UI"},
                    {TextureInternalFormat::RGB8I, "RGB8I"},
                    {TextureInternalFormat::RGB8UI, "RGB8UI"},
                    {TextureInternalFormat::RGB16I, "RGB16I"},
                    {TextureInternalFormat::RGB16UI, "RGB16UI"},
                    {TextureInternalFormat::RGB32I, "RGB32I"},
                    {TextureInternalFormat::RGB32UI, "RGB32UI"},
                    {TextureInternalFormat::RGBA8I, "RGBA8I"},
                    {TextureInternalFormat::RGBA8UI, "RGBA8UI"},
                    {TextureInternalFormat::RGBA16I, "RGBA16I"},
                    {TextureInternalFormat::RGBA16UI, "RGBA16UI"},
                    {TextureInternalFormat::RGBA32I, "RGBA32I"},
                    {TextureInternalFormat::RGBA32UI, "RGBA32UI"},

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