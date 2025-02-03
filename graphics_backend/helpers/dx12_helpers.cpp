#include "dx12_helpers.h"

const char* DX12Helpers::ToSemanticName(VertexAttributeSemantic semantic)
{
    switch (semantic)
    {
        case VertexAttributeSemantic::POSITION:
            return "POSITION";
        case VertexAttributeSemantic::TEXCOORD:
            return "TEXCOORD";
        case VertexAttributeSemantic::NORMAL:
            return "NORMAL";
        case VertexAttributeSemantic::TANGENT:
            return "TANGENT";
    }
}

DXGI_FORMAT DX12Helpers::ToVertexAttributeDataType(VertexAttributeDataType dataType, int dimensions, bool normalized)
{
    switch (dataType)
    {
        case VertexAttributeDataType::BYTE:
            switch (dimensions)
            {
                case 1:
                    return normalized ? DXGI_FORMAT_R8_SNORM : DXGI_FORMAT_R8_SINT;
                case 2:
                    return normalized ? DXGI_FORMAT_R8G8_SNORM : DXGI_FORMAT_R8G8_SINT;
                case 4:
                    return normalized ? DXGI_FORMAT_R8G8B8A8_SNORM : DXGI_FORMAT_R8G8B8A8_SINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::UNSIGNED_BYTE:
            switch (dimensions)
            {
                case 1:
                    return normalized ? DXGI_FORMAT_R8_UNORM : DXGI_FORMAT_R8_UINT;
                case 2:
                    return normalized ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_R8G8_UINT;
                case 4:
                    return normalized ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::SHORT:
            switch (dimensions)
            {
                case 1:
                    return normalized ? DXGI_FORMAT_R16_SNORM : DXGI_FORMAT_R16_SINT;
                case 2:
                    return normalized ? DXGI_FORMAT_R16G16_SNORM : DXGI_FORMAT_R16G16_SINT;
                case 4:
                    return normalized ? DXGI_FORMAT_R16G16B16A16_SNORM : DXGI_FORMAT_R16G16B16A16_SINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::UNSIGNED_SHORT:
            switch (dimensions)
            {
                case 1:
                    return normalized ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R16_UINT;
                case 2:
                    return normalized ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16G16_UINT;
                case 4:
                    return normalized ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::INT:
            switch (dimensions)
            {
                case 1:
                    return DXGI_FORMAT_R32_SINT;
                case 2:
                    return DXGI_FORMAT_R32G32_SINT;
                case 3:
                    return DXGI_FORMAT_R32G32B32_SINT;
                case 4:
                    return DXGI_FORMAT_R32G32B32A32_SINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::UNSIGNED_INT:
            switch (dimensions)
            {
                case 1:
                    return DXGI_FORMAT_R32_UINT;
                case 2:
                    return DXGI_FORMAT_R32G32_UINT;
                case 3:
                    return DXGI_FORMAT_R32G32B32_UINT;
                case 4:
                    return DXGI_FORMAT_R32G32B32A32_UINT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::HALF_FLOAT:
            switch (dimensions)
            {
                case 1:
                    return DXGI_FORMAT_R16_FLOAT;
                case 2:
                    return DXGI_FORMAT_R16G16_FLOAT;
                case 4:
                    return DXGI_FORMAT_R16G16B16A16_FLOAT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::FLOAT:
            switch (dimensions)
            {
                case 1:
                    return DXGI_FORMAT_R32_FLOAT;
                case 2:
                    return DXGI_FORMAT_R32G32_FLOAT;
                case 3:
                    return DXGI_FORMAT_R32G32B32_FLOAT;
                case 4:
                    return DXGI_FORMAT_R32G32B32A32_FLOAT;
                default:
                    return DXGI_FORMAT_UNKNOWN;
            }
        case VertexAttributeDataType::UNSIGNED_INT_2_10_10_10_REV:
            return normalized ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_R10G10B10A2_UINT;
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

DXGI_FORMAT DX12Helpers::ToTextureInternalFormat(TextureInternalFormat format, bool isLinear)
{
    switch (format)
    {

        case TextureInternalFormat::R8:
            return DXGI_FORMAT_R8_UNORM;
        case TextureInternalFormat::R8_SNORM:
            return DXGI_FORMAT_R8_SNORM;
        case TextureInternalFormat::R16:
            return DXGI_FORMAT_R16_UNORM;
        case TextureInternalFormat::R16_SNORM:
            return DXGI_FORMAT_R16_SNORM;
        case TextureInternalFormat::RG8:
            return DXGI_FORMAT_R8G8_UNORM;
        case TextureInternalFormat::RG8_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
        case TextureInternalFormat::RG16:
            return DXGI_FORMAT_R16G16_UNORM;
        case TextureInternalFormat::RG16_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
        case TextureInternalFormat::RGB4:
            return DXGI_FORMAT_B4G4R4A4_UNORM;
        case TextureInternalFormat::RGB5:
            return DXGI_FORMAT_B5G6R5_UNORM;
        case TextureInternalFormat::RGB5_A1:
            return DXGI_FORMAT_B5G5R5A1_UNORM;
        case TextureInternalFormat::RGBA8:
            return isLinear ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case TextureInternalFormat::RGBA8_SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case TextureInternalFormat::RGB10_A2:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case TextureInternalFormat::RGB10_A2UI:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case TextureInternalFormat::RGBA16:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case TextureInternalFormat::R16F:
            return DXGI_FORMAT_R16_FLOAT;
        case TextureInternalFormat::RG16F:
            return DXGI_FORMAT_R16G16_FLOAT;
        case TextureInternalFormat::RGBA16F:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TextureInternalFormat::R32F:
            return DXGI_FORMAT_R32_FLOAT;
        case TextureInternalFormat::RG32F:
            return DXGI_FORMAT_R32G32_FLOAT;
        case TextureInternalFormat::RGB32F:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case TextureInternalFormat::RGBA32F:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case TextureInternalFormat::R11F_G11F_B10F:
            return DXGI_FORMAT_R11G11B10_FLOAT;
        case TextureInternalFormat::RGB9_E5:
            return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        case TextureInternalFormat::R8I:
            return DXGI_FORMAT_R8_SINT;
        case TextureInternalFormat::R8UI:
            return DXGI_FORMAT_R8_UINT;
        case TextureInternalFormat::R16I:
            return DXGI_FORMAT_R16_SINT;
        case TextureInternalFormat::R16UI:
            return DXGI_FORMAT_R16_UINT;
        case TextureInternalFormat::R32I:
            return DXGI_FORMAT_R32_SINT;
        case TextureInternalFormat::R32UI:
            return DXGI_FORMAT_R32_UINT;
        case TextureInternalFormat::RG8I:
            return DXGI_FORMAT_R8G8_SINT;
        case TextureInternalFormat::RG8UI:
            return DXGI_FORMAT_R8G8_UINT;
        case TextureInternalFormat::RG16I:
            return DXGI_FORMAT_R16G16_SINT;
        case TextureInternalFormat::RG16UI:
            return DXGI_FORMAT_R16G16_UINT;
        case TextureInternalFormat::RG32I:
            return DXGI_FORMAT_R32G32_SINT;
        case TextureInternalFormat::RG32UI:
            return DXGI_FORMAT_R32G32_UINT;
        case TextureInternalFormat::RGB32I:
            return DXGI_FORMAT_R32G32B32_SINT;
        case TextureInternalFormat::RGB32UI:
            return DXGI_FORMAT_R32G32B32_UINT;
        case TextureInternalFormat::RGBA8I:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case TextureInternalFormat::RGBA8UI:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case TextureInternalFormat::RGBA16I:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case TextureInternalFormat::RGBA16UI:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case TextureInternalFormat::RGBA32I:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case TextureInternalFormat::RGBA32UI:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case TextureInternalFormat::BC1_RGBA:
            return isLinear ? DXGI_FORMAT_BC1_UNORM : DXGI_FORMAT_BC1_UNORM_SRGB;
        case TextureInternalFormat::BC2:
            return isLinear ? DXGI_FORMAT_BC2_UNORM : DXGI_FORMAT_BC2_UNORM_SRGB;
        case TextureInternalFormat::BC3:
            return isLinear ? DXGI_FORMAT_BC3_UNORM : DXGI_FORMAT_BC3_UNORM_SRGB;
        case TextureInternalFormat::BC4:
            return DXGI_FORMAT_BC4_UNORM;
        case TextureInternalFormat::BC5:
            return DXGI_FORMAT_BC5_UNORM;
        case TextureInternalFormat::BC6H:
            return DXGI_FORMAT_BC6H_UF16;
        case TextureInternalFormat::BC7:
            return isLinear ? DXGI_FORMAT_BC7_UNORM : DXGI_FORMAT_BC7_UNORM_SRGB;
        case TextureInternalFormat::DEPTH_32:
            return DXGI_FORMAT_D32_FLOAT;
        case TextureInternalFormat::DEPTH_16:
            return DXGI_FORMAT_D16_UNORM;
        case TextureInternalFormat::DEPTH_32_STENCIL_8:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case TextureInternalFormat::DEPTH_24_STENCIL_8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TextureInternalFormat::BGRA8:
            return isLinear ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case TextureInternalFormat::INVALID:
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

D3D12_BLEND DX12Helpers::ToBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::ZERO:
            return D3D12_BLEND_ZERO;
        case BlendFactor::ONE:
            return D3D12_BLEND_ONE;
        case BlendFactor::SRC_COLOR:
            return D3D12_BLEND_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFactor::DST_COLOR:
            return D3D12_BLEND_DEST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR:
            return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return D3D12_BLEND_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:
            return D3D12_BLEND_DEST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA:
            return D3D12_BLEND_INV_DEST_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE:
            return D3D12_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::SRC1_COLOR:
            return D3D12_BLEND_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR:
            return D3D12_BLEND_INV_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA:
            return D3D12_BLEND_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:
            return D3D12_BLEND_INV_SRC1_ALPHA;
        default:
            return D3D12_BLEND_ZERO;
    }
}