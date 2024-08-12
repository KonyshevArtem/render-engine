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
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return MTL::PixelFormat::PixelFormatBC1_RGBA;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return MTL::PixelFormat::PixelFormatBC2_RGBA;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return MTL::PixelFormat::PixelFormatBC3_RGBA;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return MTL::PixelFormat::PixelFormatBC1_RGBA_sRGB;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return MTL::PixelFormat::PixelFormatBC2_RGBA_sRGB;
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return MTL::PixelFormat::PixelFormatBC3_RGBA_sRGB;
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
