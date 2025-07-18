#include "texture_compressor_formats.h"

#include <vector>

namespace TextureCompressorFormats
{
    std::vector<TextureTypeInfo> textureTypesInfo =
            {
                    {"2D", TextureType::TEXTURE_2D, cuttlefish::Texture::Dimension::Dim2D, 1},
                    {"Cubemap", TextureType::TEXTURE_CUBEMAP, cuttlefish::Texture::Dimension::Cube, 6},
            };

    std::vector<TextureFormatInfo> textureFormatsInfo =
            {
                    {TextureInternalFormat::R8, "R8", cuttlefish::Texture::Format::R8, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::R8_SNORM, "R8_SNORM", cuttlefish::Texture::Format::R8, cuttlefish::Texture::Type::SNorm},
                    {TextureInternalFormat::R16, "R16", cuttlefish::Texture::Format::R16, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::R16_SNORM, "R16_SNORM", cuttlefish::Texture::Format::R16, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RG8, "RG8", cuttlefish::Texture::Format::R8G8, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RG8_SNORM, "RG8_SNORM", cuttlefish::Texture::Format::R8G8, cuttlefish::Texture::Type::SNorm},
                    {TextureInternalFormat::RG16, "RG16", cuttlefish::Texture::Format::R16G16, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RG16_SNORM, "RG16_SNORM", cuttlefish::Texture::Format::R16G16, cuttlefish::Texture::Type::SNorm},
                    {TextureInternalFormat::RGB4, "RGB4", cuttlefish::Texture::Format::R4G4B4A4, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGB5, "RGB5", cuttlefish::Texture::Format::R5G6B5, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGB8, "RGB8", cuttlefish::Texture::Format::R8G8B8, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGB8_SNORM, "RGB8_SNORM", cuttlefish::Texture::Format::R8G8B8, cuttlefish::Texture::Type::SNorm},
                    {TextureInternalFormat::RGB16, "RGB16", cuttlefish::Texture::Format::R16G16B16, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGBA4, "RGBA4", cuttlefish::Texture::Format::R4G4B4A4, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGB5_A1, "RGB5_A1", cuttlefish::Texture::Format::R5G5B5A1, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGBA8, "RGBA8", cuttlefish::Texture::Format::R8G8B8A8, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::RGBA8_SNORM, "RGBA8_SNORM", cuttlefish::Texture::Format::R8G8B8A8, cuttlefish::Texture::Type::SNorm},
                    {TextureInternalFormat::RGBA16, "RGBA16", cuttlefish::Texture::Format::R16G16B16A16, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::R16F, "R16F", cuttlefish::Texture::Format::R16, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RG16F, "RG16F", cuttlefish::Texture::Format::R16G16, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RGB16F, "RGB16F", cuttlefish::Texture::Format::R16G16B16, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RGBA16F, "RGBA16F", cuttlefish::Texture::Format::R16G16B16A16, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::R32F, "R32F", cuttlefish::Texture::Format::R32, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RG32F, "RG32F", cuttlefish::Texture::Format::R32G32, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RGB32F, "RGB32F", cuttlefish::Texture::Format::R32G32B32, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::RGBA32F, "RGBA32F", cuttlefish::Texture::Format::R32G32B32A32, cuttlefish::Texture::Type::Float},
                    {TextureInternalFormat::R8I, "R8I", cuttlefish::Texture::Format::R8, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::R8UI, "R8UI", cuttlefish::Texture::Format::R8, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::R16I, "R16I", cuttlefish::Texture::Format::R16, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::R16UI, "R16UI", cuttlefish::Texture::Format::R16, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::R32I, "R32I", cuttlefish::Texture::Format::R32, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::R32UI, "R32UI", cuttlefish::Texture::Format::R32, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RG8I, "RG8I", cuttlefish::Texture::Format::R8G8, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RG8UI, "RG8UI", cuttlefish::Texture::Format::R8G8, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RG16I, "RG16I", cuttlefish::Texture::Format::R16G16, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RG16UI, "RG16UI", cuttlefish::Texture::Format::R16G16, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RG32I, "RG32I", cuttlefish::Texture::Format::R32G32, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RG32UI, "RG32UI", cuttlefish::Texture::Format::R32G32, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGB8I, "RGB8I", cuttlefish::Texture::Format::R8G8B8, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGB8UI, "RGB8UI", cuttlefish::Texture::Format::R8G8B8, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGB16I, "RGB16I", cuttlefish::Texture::Format::R16G16B16, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGB16UI, "RGB16UI", cuttlefish::Texture::Format::R16G16B16, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGB32I, "RGB32I", cuttlefish::Texture::Format::R32G32B32, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGB32UI, "RGB32UI", cuttlefish::Texture::Format::R32G32B32, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGBA8I, "RGBA8I", cuttlefish::Texture::Format::R8G8B8A8, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGBA8UI, "RGBA8UI", cuttlefish::Texture::Format::R8G8B8A8, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGBA16I, "RGBA16I", cuttlefish::Texture::Format::R16G16B16A16, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGBA16UI, "RGBA16UI", cuttlefish::Texture::Format::R16G16B16A16, cuttlefish::Texture::Type::UInt},
                    {TextureInternalFormat::RGBA32I, "RGBA32I", cuttlefish::Texture::Format::R32G32B32A32, cuttlefish::Texture::Type::Int},
                    {TextureInternalFormat::RGBA32UI, "RGBA32UI", cuttlefish::Texture::Format::R32G32B32A32, cuttlefish::Texture::Type::UInt},

                    {TextureInternalFormat::BC1_RGB, "BC1_RGB", cuttlefish::Texture::Format::BC1_RGB, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC1_RGBA, "BC1_RGBA", cuttlefish::Texture::Format::BC1_RGBA, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC2, "BC2", cuttlefish::Texture::Format::BC2, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC3, "BC3", cuttlefish::Texture::Format::BC3, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC4, "BC4", cuttlefish::Texture::Format::BC4, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC5, "BC5", cuttlefish::Texture::Format::BC5, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC6H, "BC6H", cuttlefish::Texture::Format::BC6H, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::BC7, "BC7", cuttlefish::Texture::Format::BC7, cuttlefish::Texture::Type::UNorm},

                    {TextureInternalFormat::ASTC_4X4, "ASTC_4X4", cuttlefish::Texture::Format::ASTC_4x4, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::ASTC_5X5, "ASTC_5X5", cuttlefish::Texture::Format::ASTC_5x5, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::ASTC_6X6, "ASTC_6X6", cuttlefish::Texture::Format::ASTC_6x6, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::ASTC_8X8, "ASTC_8X8", cuttlefish::Texture::Format::ASTC_8x8, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::ASTC_10X10, "ASTC_10X10", cuttlefish::Texture::Format::ASTC_10x10, cuttlefish::Texture::Type::UNorm},
                    {TextureInternalFormat::ASTC_12X12, "ASTC_12X12", cuttlefish::Texture::Format::ASTC_12x12, cuttlefish::Texture::Type::UNorm},
            };

    const TextureFormatInfo& GetTextureFormatInfo(const std::string& textureFormat)
    {
        const static TextureFormatInfo invalid{.Format = TextureInternalFormat::INVALID};

        for (const TextureFormatInfo& formatInfo: textureFormatsInfo)
        {
            if (formatInfo.Name == textureFormat)
            {
                return formatInfo;
            }
        }

        return invalid;
    }

    const TextureTypeInfo& GetTextureTypeInfo(const std::string& textureType)
    {
        const static TextureTypeInfo invalid{.Count = -1};

        for (const TextureTypeInfo& typeInfo: textureTypesInfo)
        {
            if (typeInfo.Name == textureType)
            {
                return typeInfo;
            }
        }

        return invalid;
    }
}