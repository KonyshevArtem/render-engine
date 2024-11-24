#ifdef RENDER_BACKEND_METAL

#include "metal_helpers.h"

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
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

MTL::PixelFormat MetalHelpers::ToTextureInternalFormat(TextureInternalFormat format, bool isLinear)
{
    switch (format)
    {
        case TextureInternalFormat::R8:
            return isLinear ? MTL::PixelFormat::PixelFormatR8Unorm : MTL::PixelFormat::PixelFormatR8Unorm_sRGB;
        case TextureInternalFormat::R8_SNORM:
            return MTL::PixelFormat::PixelFormatR8Snorm;
        case TextureInternalFormat::R16:
            return MTL::PixelFormat::PixelFormatR16Unorm;
        case TextureInternalFormat::R16_SNORM:
            return MTL::PixelFormat::PixelFormatR16Snorm;
        case TextureInternalFormat::RG8:
            return isLinear ? MTL::PixelFormat::PixelFormatRG8Unorm : MTL::PixelFormat::PixelFormatRG8Unorm_sRGB;
        case TextureInternalFormat::RG8_SNORM:
            return MTL::PixelFormat::PixelFormatRG8Snorm;
        case TextureInternalFormat::RG16:
            return MTL::PixelFormat::PixelFormatRG16Unorm;
        case TextureInternalFormat::RG16_SNORM:
            return MTL::PixelFormat::PixelFormatRG16Snorm;
        case TextureInternalFormat::RGBA8:
            return isLinear ? MTL::PixelFormat::PixelFormatRGBA8Unorm : MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB;
        case TextureInternalFormat::RGBA8_SNORM:
            return MTL::PixelFormat::PixelFormatRGBA8Snorm;
        case TextureInternalFormat::RGB10_A2:
            return MTL::PixelFormat::PixelFormatRGB10A2Unorm;
        case TextureInternalFormat::RGB10_A2UI:
            return MTL::PixelFormat::PixelFormatRGB10A2Uint;
        case TextureInternalFormat::RGBA16:
            return MTL::PixelFormat::PixelFormatRGBA16Unorm;
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
        case TextureInternalFormat::BGRA8:
            return MTL::PixelFormat::PixelFormatBGRA8Unorm;
        case TextureInternalFormat::BGRA8_SNORM:
            return MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB;
        case TextureInternalFormat::DEPTH_32:
            return MTL::PixelFormat::PixelFormatDepth32Float;
        case TextureInternalFormat::DEPTH_16:
            return MTL::PixelFormat::PixelFormatDepth16Unorm;
        case TextureInternalFormat::DEPTH_32_STENCIL_8:
            return MTL::PixelFormat::PixelFormatDepth32Float_Stencil8;
        case TextureInternalFormat::DEPTH_24_STENCIL_8:
            return MTL::PixelFormat::PixelFormatDepth24Unorm_Stencil8;
        case TextureInternalFormat::BC1_RGBA:
            return isLinear ? MTL::PixelFormat::PixelFormatBC1_RGBA : MTL::PixelFormat::PixelFormatBC1_RGBA_sRGB;
        case TextureInternalFormat::BC2:
            return isLinear ? MTL::PixelFormat::PixelFormatBC2_RGBA : MTL::PixelFormat::PixelFormatBC2_RGBA_sRGB;
        case TextureInternalFormat::BC3:
            return isLinear ? MTL::PixelFormat::PixelFormatBC3_RGBA : MTL::PixelFormat::PixelFormatBC3_RGBA_sRGB;
        case TextureInternalFormat::BC4:
            return MTL::PixelFormat::PixelFormatBC4_RUnorm;
        case TextureInternalFormat::BC5:
            return MTL::PixelFormat::PixelFormatBC5_RGUnorm;
        case TextureInternalFormat::BC6H:
            return MTL::PixelFormat::PixelFormatBC6H_RGBFloat;
        case TextureInternalFormat::BC7:
            return isLinear ? MTL::PixelFormat::PixelFormatBC7_RGBAUnorm : MTL::PixelFormat::PixelFormatBC7_RGBAUnorm_sRGB;
        case TextureInternalFormat::ASTC_4X4:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_4x4_LDR : MTL::PixelFormat::PixelFormatASTC_4x4_sRGB;
        case TextureInternalFormat::ASTC_5X5:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_5x5_LDR : MTL::PixelFormat::PixelFormatASTC_5x5_sRGB;
        case TextureInternalFormat::ASTC_6X6:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_6x6_LDR : MTL::PixelFormat::PixelFormatASTC_6x6_sRGB;
        case TextureInternalFormat::ASTC_8X8:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_8x8_LDR : MTL::PixelFormat::PixelFormatASTC_8x8_sRGB;
        case TextureInternalFormat::ASTC_10X10:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_10x10_LDR : MTL::PixelFormat::PixelFormatASTC_10x10_sRGB;
        case TextureInternalFormat::ASTC_12X12:
            return isLinear ? MTL::PixelFormat::PixelFormatASTC_12x12_LDR : MTL::PixelFormat::PixelFormatASTC_12x12_sRGB;
        case TextureInternalFormat::INVALID:
        default:
            return MTL::PixelFormat::PixelFormatInvalid;
    }
}

TextureInternalFormat MetalHelpers::FromTextureInternalFormat(MTL::PixelFormat format, bool& outIsLinear)
{
    outIsLinear = !(format == MTL::PixelFormat::PixelFormatR8Unorm_sRGB |
                    format == MTL::PixelFormat::PixelFormatRG8Unorm_sRGB |
                    format == MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB |
                    format == MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB |
                    format == MTL::PixelFormat::PixelFormatBC1_RGBA_sRGB |
                    format == MTL::PixelFormat::PixelFormatBC2_RGBA_sRGB |
                    format == MTL::PixelFormat::PixelFormatBC3_RGBA_sRGB |
                    format == MTL::PixelFormat::PixelFormatBC7_RGBAUnorm_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_4x4_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_5x5_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_6x6_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_8x8_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_10x10_sRGB |
                    format == MTL::PixelFormat::PixelFormatASTC_12x12_sRGB);

    switch (format)
    {
        case MTL::PixelFormat::PixelFormatR8Unorm:
            return TextureInternalFormat::R8;
        case MTL::PixelFormat::PixelFormatR8Snorm:
            return TextureInternalFormat::R8_SNORM;
        case MTL::PixelFormat::PixelFormatR16Unorm:
            return TextureInternalFormat::R16;
        case MTL::PixelFormat::PixelFormatR16Snorm:
            return TextureInternalFormat::R16_SNORM;
        case MTL::PixelFormat::PixelFormatRG8Unorm:
            return TextureInternalFormat::RG8;
        case MTL::PixelFormat::PixelFormatRG8Snorm:
            return TextureInternalFormat::RG8_SNORM;
        case MTL::PixelFormat::PixelFormatRG16Unorm:
            return TextureInternalFormat::RG16;
        case MTL::PixelFormat::PixelFormatRG16Snorm:
            return TextureInternalFormat::RG16_SNORM;
        case MTL::PixelFormat::PixelFormatRGBA8Unorm:
            return TextureInternalFormat::RGBA8;
        case MTL::PixelFormat::PixelFormatRGBA8Snorm:
            return TextureInternalFormat::RGBA8_SNORM;
        case MTL::PixelFormat::PixelFormatRGB10A2Unorm:
            return TextureInternalFormat::RGB10_A2;
        case MTL::PixelFormat::PixelFormatRGB10A2Uint:
            return TextureInternalFormat::RGB10_A2UI;
        case MTL::PixelFormat::PixelFormatRGBA16Unorm:
            return TextureInternalFormat::RGBA16;
        case MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB:
            return TextureInternalFormat::RGBA8;
        case MTL::PixelFormat::PixelFormatR16Float:
            return TextureInternalFormat::R16F;
        case MTL::PixelFormat::PixelFormatRG16Float:
            return TextureInternalFormat::RG16F;
        case MTL::PixelFormat::PixelFormatRGBA16Float:
            return TextureInternalFormat::RGBA16F;
        case MTL::PixelFormat::PixelFormatR32Float:
            return TextureInternalFormat::R32F;
        case MTL::PixelFormat::PixelFormatRG32Float:
            return TextureInternalFormat::RG32F;
        case MTL::PixelFormat::PixelFormatRGBA32Float:
            return TextureInternalFormat::RGBA32F;
        case MTL::PixelFormat::PixelFormatRG11B10Float:
            return TextureInternalFormat::R11F_G11F_B10F;
        case MTL::PixelFormat::PixelFormatRGB9E5Float:
            return TextureInternalFormat::RGB9_E5;
        case MTL::PixelFormat::PixelFormatR8Sint:
            return TextureInternalFormat::R8I;
        case MTL::PixelFormat::PixelFormatR8Uint:
            return TextureInternalFormat::R8UI;
        case MTL::PixelFormat::PixelFormatR16Sint:
            return TextureInternalFormat::R16I;
        case MTL::PixelFormat::PixelFormatR16Uint:
            return TextureInternalFormat::R16UI;
        case MTL::PixelFormat::PixelFormatR32Sint:
            return TextureInternalFormat::R32I;
        case MTL::PixelFormat::PixelFormatR32Uint:
            return TextureInternalFormat::R32UI;
        case MTL::PixelFormat::PixelFormatRG8Sint:
            return TextureInternalFormat::RG8I;
        case MTL::PixelFormat::PixelFormatRG8Uint:
            return TextureInternalFormat::RG8UI;
        case MTL::PixelFormat::PixelFormatRG16Sint:
            return TextureInternalFormat::RG16I;
        case MTL::PixelFormat::PixelFormatRG16Uint:
            return TextureInternalFormat::RG16UI;
        case MTL::PixelFormat::PixelFormatRG32Sint:
            return TextureInternalFormat::RG32I;
        case MTL::PixelFormat::PixelFormatRG32Uint:
            return TextureInternalFormat::RG32UI;
        case MTL::PixelFormat::PixelFormatRGBA8Sint:
            return TextureInternalFormat::RGBA8I;
        case MTL::PixelFormat::PixelFormatRGBA8Uint:
            return TextureInternalFormat::RGBA8UI;
        case MTL::PixelFormat::PixelFormatRGBA16Sint:
            return TextureInternalFormat::RGBA16I;
        case MTL::PixelFormat::PixelFormatRGBA16Uint:
            return TextureInternalFormat::RGBA16UI;
        case MTL::PixelFormat::PixelFormatRGBA32Sint:
            return TextureInternalFormat::RGBA32I;
        case MTL::PixelFormat::PixelFormatRGBA32Uint:
            return TextureInternalFormat::RGBA32UI;
        case MTL::PixelFormat::PixelFormatBGRA8Unorm:
            return TextureInternalFormat::BGRA8;
        case MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB:
            return TextureInternalFormat::BGRA8_SNORM;
        case MTL::PixelFormat::PixelFormatDepth32Float:
            return TextureInternalFormat::DEPTH_32;
        case MTL::PixelFormat::PixelFormatDepth16Unorm:
            return TextureInternalFormat::DEPTH_16;
        case MTL::PixelFormat::PixelFormatDepth32Float_Stencil8:
            return TextureInternalFormat::DEPTH_32_STENCIL_8;
        case MTL::PixelFormat::PixelFormatDepth24Unorm_Stencil8:
            return TextureInternalFormat::DEPTH_24_STENCIL_8;
        case MTL::PixelFormat::PixelFormatBC1_RGBA:
        case MTL::PixelFormat::PixelFormatBC1_RGBA_sRGB:
            return TextureInternalFormat::BC1_RGBA;
        case MTL::PixelFormat::PixelFormatBC2_RGBA:
        case MTL::PixelFormat::PixelFormatBC2_RGBA_sRGB:
            return TextureInternalFormat::BC2;
        case MTL::PixelFormat::PixelFormatBC3_RGBA:
        case MTL::PixelFormat::PixelFormatBC3_RGBA_sRGB:
            return TextureInternalFormat::BC3;
        case MTL::PixelFormat::PixelFormatBC4_RUnorm:
        case MTL::PixelFormat::PixelFormatBC4_RSnorm:
            return TextureInternalFormat::BC4;
        case MTL::PixelFormat::PixelFormatBC5_RGUnorm:
        case MTL::PixelFormat::PixelFormatBC5_RGSnorm:
            return TextureInternalFormat::BC5;
        case MTL::PixelFormat::PixelFormatBC6H_RGBFloat:
        case MTL::PixelFormat::PixelFormatBC6H_RGBUfloat:
            return TextureInternalFormat::BC6H;
        case MTL::PixelFormat::PixelFormatBC7_RGBAUnorm:
        case MTL::PixelFormat::PixelFormatBC7_RGBAUnorm_sRGB:
            return TextureInternalFormat::BC7;
        case MTL::PixelFormat::PixelFormatASTC_4x4_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_4x4_LDR:
            return TextureInternalFormat::ASTC_4X4;
        case MTL::PixelFormat::PixelFormatASTC_5x5_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_5x5_LDR:
            return TextureInternalFormat::ASTC_5X5;
        case MTL::PixelFormat::PixelFormatASTC_6x6_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_6x6_LDR:
            return TextureInternalFormat::ASTC_6X6;
        case MTL::PixelFormat::PixelFormatASTC_8x8_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_8x8_LDR:
            return TextureInternalFormat::ASTC_8X8;
        case MTL::PixelFormat::PixelFormatASTC_10x10_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_10x10_LDR:
            return TextureInternalFormat::ASTC_10X10;
        case MTL::PixelFormat::PixelFormatASTC_12x12_sRGB:
        case MTL::PixelFormat::PixelFormatASTC_12x12_LDR:
            return TextureInternalFormat::ASTC_12X12;
        case MTL::PixelFormat::PixelFormatInvalid:
            return TextureInternalFormat::INVALID;
        default:
            return TextureInternalFormat::RGBA8;
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

MTL::VertexFormat MetalHelpers::ToVertexFormat(VertexAttributeDataType dataType, int dimensions, bool normalized)
{
    switch (dataType)
    {
        case VertexAttributeDataType::BYTE:
            switch (dimensions)
            {
                case 1:
                    return normalized ? MTL::VertexFormat::VertexFormatCharNormalized : MTL::VertexFormat::VertexFormatChar;
                case 2:
                    return normalized ? MTL::VertexFormat::VertexFormatChar2Normalized : MTL::VertexFormat::VertexFormatChar2;
                case 3:
                    return normalized ? MTL::VertexFormat::VertexFormatChar3Normalized : MTL::VertexFormat::VertexFormatChar3;
                case 4:
                    return normalized ? MTL::VertexFormat::VertexFormatChar4Normalized : MTL::VertexFormat::VertexFormatChar4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::UNSIGNED_BYTE:
            switch (dimensions)
            {
                case 1:
                    return normalized ? MTL::VertexFormat::VertexFormatUCharNormalized : MTL::VertexFormat::VertexFormatUChar;
                case 2:
                    return normalized ? MTL::VertexFormat::VertexFormatUChar2Normalized : MTL::VertexFormat::VertexFormatUChar2;
                case 3:
                    return normalized ? MTL::VertexFormat::VertexFormatUChar3Normalized : MTL::VertexFormat::VertexFormatUChar3;
                case 4:
                    return normalized ? MTL::VertexFormat::VertexFormatUChar4Normalized : MTL::VertexFormat::VertexFormatUChar4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::SHORT:
            switch (dimensions)
            {
                case 1:
                    return normalized ? MTL::VertexFormat::VertexFormatShortNormalized : MTL::VertexFormat::VertexFormatShort;
                case 2:
                    return normalized ? MTL::VertexFormat::VertexFormatShort2Normalized : MTL::VertexFormat::VertexFormatShort2;
                case 3:
                    return normalized ? MTL::VertexFormat::VertexFormatShort3Normalized : MTL::VertexFormat::VertexFormatShort3;
                case 4:
                    return normalized ? MTL::VertexFormat::VertexFormatShort4Normalized : MTL::VertexFormat::VertexFormatShort4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::UNSIGNED_SHORT:
            switch (dimensions)
            {
                case 1:
                    return normalized ? MTL::VertexFormat::VertexFormatUShortNormalized : MTL::VertexFormat::VertexFormatUShort;
                case 2:
                    return normalized ? MTL::VertexFormat::VertexFormatUShort2Normalized : MTL::VertexFormat::VertexFormatUShort2;
                case 3:
                    return normalized ? MTL::VertexFormat::VertexFormatUShort3Normalized : MTL::VertexFormat::VertexFormatUShort3;
                case 4:
                    return normalized ? MTL::VertexFormat::VertexFormatUShort4Normalized : MTL::VertexFormat::VertexFormatUShort4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::INT:
            switch (dimensions)
            {
                case 1:
                    return MTL::VertexFormat::VertexFormatInt;
                case 2:
                    return MTL::VertexFormat::VertexFormatInt2;
                case 3:
                    return MTL::VertexFormat::VertexFormatInt3;
                case 4:
                    return MTL::VertexFormat::VertexFormatInt4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::UNSIGNED_INT:
            switch (dimensions)
            {
                case 1:
                    return MTL::VertexFormat::VertexFormatUInt;
                case 2:
                    return MTL::VertexFormat::VertexFormatUInt2;
                case 3:
                    return MTL::VertexFormat::VertexFormatUInt3;
                case 4:
                    return MTL::VertexFormat::VertexFormatUInt4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::HALF_FLOAT:
            switch (dimensions)
            {
                case 1:
                    return MTL::VertexFormat::VertexFormatHalf;
                case 2:
                    return MTL::VertexFormat::VertexFormatHalf2;
                case 3:
                    return MTL::VertexFormat::VertexFormatHalf3;
                case 4:
                    return MTL::VertexFormat::VertexFormatHalf4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::FLOAT:
            switch (dimensions)
            {
                case 1:
                    return MTL::VertexFormat::VertexFormatFloat;
                case 2:
                    return MTL::VertexFormat::VertexFormatFloat2;
                case 3:
                    return MTL::VertexFormat::VertexFormatFloat3;
                case 4:
                    return MTL::VertexFormat::VertexFormatFloat4;
                default:
                    return MTL::VertexFormat::VertexFormatInvalid;
            }
        case VertexAttributeDataType::INT_2_10_10_10_REV:
            return MTL::VertexFormat::VertexFormatInt1010102Normalized;
        case VertexAttributeDataType::UNSIGNED_INT_2_10_10_10_REV:
            return MTL::VertexFormat::VertexFormatUInt1010102Normalized;
        default:
            return MTL::VertexFormatInvalid;
    }
}

MTL::LoadAction MetalHelpers::ToLoadAction(LoadAction loadAction)
{
    switch (loadAction)
    {
        case LoadAction::DONT_CARE:
            return MTL::LoadAction::LoadActionDontCare;
        case LoadAction::LOAD:
            return MTL::LoadAction::LoadActionLoad;
        case LoadAction::CLEAR:
            return MTL::LoadAction::LoadActionClear;
        default:
            return MTL::LoadAction::LoadActionClear;
    }
}

MTL::StoreAction MetalHelpers::ToStoreAction(StoreAction storeAction)
{
    switch (storeAction)
    {
        case StoreAction::DONT_CARE:
            return MTL::StoreAction::StoreActionDontCare;
        case StoreAction::STORE:
            return MTL::StoreAction::StoreActionStore;
        case StoreAction::MULTISAMPLE_RESOLVE:
            return MTL::StoreAction::StoreActionMultisampleResolve;
        case StoreAction::STORE_AND_MULTISAMPLE_RESOLVE:
            return MTL::StoreAction::StoreActionStoreAndMultisampleResolve;
        case StoreAction::UNKNOWN:
            return MTL::StoreAction::StoreActionUnknown;
        case StoreAction::CUSTOM_SAMPLE_DEPTH_STORE:
            return MTL::StoreAction::StoreActionCustomSampleDepthStore;
        default:
            return MTL::StoreAction::StoreActionStore;
    }
}

MTL::CompareFunction MetalHelpers::ToDepthCompareFunction(DepthFunction function)
{
    switch (function)
    {
        case DepthFunction::NEVER:
            return MTL::CompareFunction::CompareFunctionNever;
        case DepthFunction::LESS:
            return MTL::CompareFunction::CompareFunctionLess;
        case DepthFunction::EQUAL:
            return MTL::CompareFunction::CompareFunctionEqual;
        case DepthFunction::LEQUAL:
            return MTL::CompareFunction::CompareFunctionLessEqual;
        case DepthFunction::GREATER:
            return MTL::CompareFunction::CompareFunctionGreater;
        case DepthFunction::NOTEQUAL:
            return MTL::CompareFunction::CompareFunctionNotEqual;
        case DepthFunction::GEQUAL:
            return MTL::CompareFunction::CompareFunctionGreaterEqual;
        case DepthFunction::ALWAYS:
            return MTL::CompareFunction::CompareFunctionAlways;
    }
}

MTL::Winding MetalHelpers::ToCullFaceOrientation(CullFaceOrientation orientation)
{
    return orientation == CullFaceOrientation::CLOCKWISE ? MTL::Winding::WindingClockwise : MTL::Winding::WindingCounterClockwise;
}

MTL::CullMode MetalHelpers::ToCullFace(CullFace face)
{
    switch (face)
    {
        case CullFace::NONE:
            return MTL::CullMode::CullModeNone;
        case CullFace::FRONT:
            return MTL::CullMode::CullModeFront;
        case CullFace::BACK:
            return MTL::CullMode::CullModeBack;
    }
}

MTL::BlendFactor MetalHelpers::ToBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::ZERO:
            return MTL::BlendFactor::BlendFactorZero;
        case BlendFactor::ONE:
            return MTL::BlendFactor::BlendFactorOne;
        case BlendFactor::SRC_COLOR:
            return MTL::BlendFactor::BlendFactorSourceColor;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return MTL::BlendFactor::BlendFactorOneMinusSourceColor;
        case BlendFactor::DST_COLOR:
            return MTL::BlendFactor::BlendFactorDestinationColor;
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return MTL::BlendFactor::BlendFactorOneMinusDestinationColor;
        case BlendFactor::SRC_ALPHA:
            return MTL::BlendFactor::BlendFactorSourceAlpha;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return MTL::BlendFactor::BlendFactorOneMinusSourceAlpha;
        case BlendFactor::DST_ALPHA:
            return MTL::BlendFactor::BlendFactorDestinationAlpha;
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return MTL::BlendFactor::BlendFactorOneMinusDestinationAlpha;
        case BlendFactor::CONSTANT_COLOR:
            return MTL::BlendFactor::BlendFactorBlendColor;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
            return MTL::BlendFactor::BlendFactorOneMinusBlendColor;
        case BlendFactor::CONSTANT_ALPHA:
            return MTL::BlendFactor::BlendFactorBlendAlpha;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
            return MTL::BlendFactor::BlendFactorOneMinusBlendAlpha;
        case BlendFactor::SRC_ALPHA_SATURATE:
            return MTL::BlendFactor::BlendFactorSourceAlphaSaturated;
        case BlendFactor::SRC1_COLOR:
            return MTL::BlendFactor::BlendFactorSource1Color;
        case BlendFactor::ONE_MINUS_SRC1_COLOR:
            return MTL::BlendFactor::BlendFactorOneMinusSource1Color;
        case BlendFactor::SRC1_ALPHA:
            return MTL::BlendFactor::BlendFactorSource1Alpha;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:
            return MTL::BlendFactor::BlendFactorOneMinusSource1Alpha;
    }
}

TextureDataType MetalHelpers::FromTextureDataType(MTL::DataType dataType, MTL::TextureType textureType)
{
    switch (textureType)
    {
        case MTL::TextureType::TextureType1D:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_1D;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_1D;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_1D;
            }
        }
        case MTL::TextureType::TextureType1DArray:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_1D_ARRAY;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_1D_ARRAY;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_1D_ARRAY;
            }
        }
        case MTL::TextureType::TextureType2D:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_2D;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_2D;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_2D;
            }
        }
        case MTL::TextureType::TextureType2DMultisample:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_2D_MULTISAMPLE;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_2D_MULTISAMPLE;
            }
        }
        case MTL::TextureType::TextureType2DArray:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_2D_ARRAY;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_2D_ARRAY;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_2D_ARRAY;
            }
        }
        case MTL::TextureType::TextureType2DMultisampleArray:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_2D_MULTISAMPLE_ARRAY;
            }
        }
        case MTL::TextureType::TextureType3D:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_3D;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_3D;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_3D;
            }
        }
        case MTL::TextureType::TextureTypeCube:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_CUBE;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_CUBE;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_CUBE;
            }
        }
        case MTL::TextureType::TextureTypeTextureBuffer:
        {
            switch (dataType)
            {
                case MTL::DataType::DataTypeInt:
                    return TextureDataType::INT_SAMPLER_BUFFER;
                case MTL::DataType::DataTypeUInt:
                    return TextureDataType::UNSIGNED_INT_SAMPLER_BUFFER;
                case MTL::DataType::DataTypeFloat:
                case MTL::DataType::DataTypeHalf:
                    return TextureDataType::SAMPLER_BUFFER;
            }
        }
    }
}

#endif // RENDER_BACKEND_METAL