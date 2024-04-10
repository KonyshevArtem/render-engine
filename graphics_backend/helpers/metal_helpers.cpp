#include "metal_helpers.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Metal/Metal.hpp"

MTL::PrimitiveType MetalHelpers::ToPrimitiveType(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case PrimitiveType::POINTS:
            return MTL::PrimitiveType::PrimitiveTypePoint;
        case PrimitiveType::LINE_STRIP:
            return MTL::PrimitiveType::PrimitiveTypeLineStrip;
        case PrimitiveType::LINES:
            return MTL::PrimitiveType::PrimitiveTypeLine;
        case PrimitiveType::TRIANGLE_STRIP:
            return MTL::PrimitiveType::PrimitiveTypeTriangleStrip;
        case PrimitiveType::TRIANGLES:
            return MTL::PrimitiveType::PrimitiveTypeTriangle;
        default:
            return MTL::PrimitiveType::PrimitiveTypeTriangle;
    }
}

MTL::IndexType MetalHelpers::ToIndicesDataType(IndicesDataType dataType)
{
    switch (dataType)
    {
        case IndicesDataType::UNSIGNED_BYTE:
        case IndicesDataType::UNSIGNED_SHORT:
            return MTL::IndexType::IndexTypeUInt16;
        case IndicesDataType::UNSIGNED_INT:
            return MTL::IndexType::IndexTypeUInt32;
    }
}

MTL::TextureType MetalHelpers::ToTextureType(TextureType textureType)
{
    switch (textureType)
    {
        case TextureType::TEXTURE_1D:
            return MTL::TextureType::TextureType1D;
        case TextureType::TEXTURE_1D_ARRAY:
            return MTL::TextureType::TextureType1DArray;
        case TextureType::TEXTURE_2D:
        case TextureType::TEXTURE_RECTANGLE:
            return MTL::TextureType::TextureType2D;
        case TextureType::TEXTURE_2D_MULTISAMPLE:
            return MTL::TextureType::TextureType2DMultisample;
        case TextureType::TEXTURE_2D_ARRAY:
            return MTL::TextureType::TextureType2DArray;
        case TextureType::TEXTURE_2D_MULTISAMPLE_ARRAY:
            return MTL::TextureType::TextureType2DMultisampleArray;
        case TextureType::TEXTURE_3D:
            return MTL::TextureType::TextureType3D;
        case TextureType::TEXTURE_CUBEMAP:
            return MTL::TextureType::TextureTypeCube;
        case TextureType::TEXTURE_CUBEMAP_ARRAY:
            return MTL::TextureType::TextureTypeCubeArray;
        case TextureType::TEXTURE_BUFFER:
            return MTL::TextureType::TextureTypeTextureBuffer;
    }
}

MTL::PixelFormat MetalHelpers::ToTextureInternalFormat(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::RED:
            return MTL::PixelFormat::PixelFormatR8Unorm;
        case TextureInternalFormat::RG:
            return MTL::PixelFormat::PixelFormatRG16Unorm;
        case TextureInternalFormat::RGBA:
            return MTL::PixelFormat::PixelFormatRGBA8Unorm;
        case TextureInternalFormat::SRGB_ALPHA:
            return MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB;

        case TextureInternalFormat::COMPRESSED_RGB:
            return MTL::PixelFormat::PixelFormatETC2_RGB8;
        case TextureInternalFormat::COMPRESSED_RGBA:
            return MTL::PixelFormat::PixelFormatBC7_RGBAUnorm;
        case TextureInternalFormat::COMPRESSED_SRGB:
            return MTL::PixelFormat::PixelFormatETC2_RGB8_sRGB;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA:
            return MTL::PixelFormat::PixelFormatBC7_RGBAUnorm_sRGB;
        case TextureInternalFormat::COMPRESSED_RED:
            return MTL::PixelFormat::PixelFormatBC4_RUnorm;
        case TextureInternalFormat::COMPRESSED_RG:
            return MTL::PixelFormat::PixelFormatBC5_RGUnorm;

        case TextureInternalFormat::R8:
            return MTL::PixelFormat::PixelFormatR8Unorm;
        case TextureInternalFormat::R8_SNORM:
            return MTL::PixelFormat::PixelFormatR8Snorm;
        case TextureInternalFormat::R16:
            return MTL::PixelFormat::PixelFormatR16Unorm;
        case TextureInternalFormat::R16_SNORM:
            return MTL::PixelFormat::PixelFormatR16Snorm;
        case TextureInternalFormat::RG8:
            return MTL::PixelFormat::PixelFormatRG8Unorm;
        case TextureInternalFormat::RG8_SNORM:
            return MTL::PixelFormat::PixelFormatRG8Snorm;
        case TextureInternalFormat::RG16:
            return MTL::PixelFormat::PixelFormatRG16Unorm;
        case TextureInternalFormat::RG16_SNORM:
            return MTL::PixelFormat::PixelFormatRG16Snorm;
        case TextureInternalFormat::RGBA8:
            return MTL::PixelFormat::PixelFormatRGBA8Unorm;
        case TextureInternalFormat::RGBA8_SNORM:
            return MTL::PixelFormat::PixelFormatRGBA8Snorm;
        case TextureInternalFormat::RGB10_A2:
            return MTL::PixelFormat::PixelFormatRGB10A2Unorm;
        case TextureInternalFormat::RGB10_A2UI:
            return MTL::PixelFormat::PixelFormatRGB10A2Uint;
        case TextureInternalFormat::RGBA16:
            return MTL::PixelFormat::PixelFormatRGBA16Unorm;
        case TextureInternalFormat::SRGB8_ALPHA8:
            return MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB;
        case TextureInternalFormat::R16F:
            return MTL::PixelFormat::PixelFormatR16Float;
        case TextureInternalFormat::RG16F:
            return MTL::PixelFormat::PixelFormatRG16Float;
        case TextureInternalFormat::RGBA16F:
            return MTL::PixelFormat::PixelFormatRGBA16Float;
        case TextureInternalFormat::R32F:
            return MTL::PixelFormat::PixelFormatR32Float;
        case TextureInternalFormat::RG32F:
            return MTL::PixelFormat::PixelFormatRG32Float;
        case TextureInternalFormat::RGBA32F:
            return MTL::PixelFormat::PixelFormatRGBA32Float;
        case TextureInternalFormat::R11F_G11F_B10F:
            return MTL::PixelFormat::PixelFormatRG11B10Float;
        case TextureInternalFormat::RGB9_E5:
            return MTL::PixelFormat::PixelFormatRGB9E5Float;
        case TextureInternalFormat::R8I:
            return MTL::PixelFormat::PixelFormatR8Sint;
        case TextureInternalFormat::R8UI:
            return MTL::PixelFormat::PixelFormatR8Uint;
        case TextureInternalFormat::R16I:
            return MTL::PixelFormat::PixelFormatR16Sint;
        case TextureInternalFormat::R16UI:
            return MTL::PixelFormat::PixelFormatR16Uint;
        case TextureInternalFormat::R32I:
            return MTL::PixelFormat::PixelFormatR32Sint;
        case TextureInternalFormat::R32UI:
            return MTL::PixelFormat::PixelFormatR32Uint;
        case TextureInternalFormat::RG8I:
            return MTL::PixelFormat::PixelFormatRG8Sint;
        case TextureInternalFormat::RG8UI:
            return MTL::PixelFormat::PixelFormatRG8Uint;
        case TextureInternalFormat::RG16I:
            return MTL::PixelFormat::PixelFormatRG16Sint;
        case TextureInternalFormat::RG16UI:
            return MTL::PixelFormat::PixelFormatRG16Uint;
        case TextureInternalFormat::RG32I:
            return MTL::PixelFormat::PixelFormatRG32Sint;
        case TextureInternalFormat::RG32UI:
            return MTL::PixelFormat::PixelFormatRG32Uint;
        case TextureInternalFormat::RGBA8I:
            return MTL::PixelFormat::PixelFormatRGBA8Sint;
        case TextureInternalFormat::RGBA8UI:
            return MTL::PixelFormat::PixelFormatRGBA8Uint;
        case TextureInternalFormat::RGBA16I:
            return MTL::PixelFormat::PixelFormatRGBA16Sint;
        case TextureInternalFormat::RGBA16UI:
            return MTL::PixelFormat::PixelFormatRGBA16Uint;
        case TextureInternalFormat::RGBA32I:
            return MTL::PixelFormat::PixelFormatRGBA32Sint;
        case TextureInternalFormat::RGBA32UI:
            return MTL::PixelFormat::PixelFormatRGBA32Uint;
        case TextureInternalFormat::DEPTH_COMPONENT:
            return MTL::PixelFormat::PixelFormatDepth32Float;
        case TextureInternalFormat::DEPTH_STENCIL:
            return MTL::PixelFormat::PixelFormatDepth32Float_Stencil8;
        default:
            return MTL::PixelFormat::PixelFormatInvalid;
    }
}

MTL::SamplerAddressMode MetalHelpers::ToTextureWrapMode(TextureWrapMode wrapMode)
{
    switch (wrapMode)
    {
        case TextureWrapMode::CLAMP_TO_EDGE:
            return MTL::SamplerAddressMode::SamplerAddressModeClampToEdge;
        case TextureWrapMode::CLAMP_TO_BORDER:
            return MTL::SamplerAddressMode::SamplerAddressModeClampToBorderColor;
        case TextureWrapMode::MIRRORED_REPEAT:
            return MTL::SamplerAddressMode::SamplerAddressModeMirrorRepeat;
        case TextureWrapMode::REPEAT:
            return MTL::SamplerAddressMode::SamplerAddressModeRepeat;
    }
}

MTL::SamplerMinMagFilter MetalHelpers::ToTextureFilteringMode(TextureFilteringMode filteringMode)
{
    switch (filteringMode)
    {
        case TextureFilteringMode::NEAREST:
        case TextureFilteringMode::NEAREST_MIPMAP_NEAREST:
        case TextureFilteringMode::NEAREST_MIPMAP_LINEAR:
            return MTL::SamplerMinMagFilter::SamplerMinMagFilterNearest;
        case TextureFilteringMode::LINEAR:
        case TextureFilteringMode::LINEAR_MIPMAP_NEAREST:
        case TextureFilteringMode::LINEAR_MIPMAP_LINEAR:
            return MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear;
    }
}

MTL::SamplerBorderColor MetalHelpers::ToTextureBorderColor(const float color[4])
{
    if (color[0] == 1 && color[1] == 1 && color[2] == 1 && color[3] == 1)
    {
        return MTL::SamplerBorderColor::SamplerBorderColorOpaqueWhite;
    }
    if (color[3] == 1)
    {
        return MTL::SamplerBorderColor::SamplerBorderColorOpaqueBlack;
    }
    return MTL::SamplerBorderColor::SamplerBorderColorTransparentBlack;
}