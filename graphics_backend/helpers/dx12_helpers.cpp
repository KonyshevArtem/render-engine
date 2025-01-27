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