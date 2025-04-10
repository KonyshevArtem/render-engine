#ifndef RENDER_ENGINE_DX12_HELPERS_H
#define RENDER_ENGINE_DX12_HELPERS_H

#if RENDER_BACKEND_DX12

#include "enums/vertex_attribute_semantic.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/texture_internal_format.h"
#include "enums/blend_factor.h"
#include "enums/primitive_type.h"

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"

namespace DX12Helpers
{
    const char* ToSemanticName(VertexAttributeSemantic semantic);
    DXGI_FORMAT ToVertexAttributeDataType(VertexAttributeDataType dataType, int dimensions, bool normalized);
    DXGI_FORMAT ToTextureInternalFormat(TextureInternalFormat format, bool isLinear);
    D3D12_BLEND ToBlendFactor(BlendFactor factor);
    D3D12_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(PrimitiveType primitiveType);
}

#endif

#endif //RENDER_ENGINE_DX12_HELPERS_H
