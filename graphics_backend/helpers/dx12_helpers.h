#ifndef RENDER_ENGINE_DX12_HELPERS_H
#define RENDER_ENGINE_DX12_HELPERS_H

#if RENDER_BACKEND_DX12

#include "enums/vertex_attribute_semantic.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/texture_internal_format.h"
#include "enums/blend_factor.h"
#include "enums/primitive_type.h"
#include "enums/texture_type.h"
#include "enums/texture_filtering_mode.h"
#include "enums/texture_wrap_mode.h"
#include "enums/cull_face.h"
#include "enums/comparison_function.h"
#include "enums/resource_state.h"

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "pix.h"

namespace DX12Helpers
{
    const char* ToSemanticName(VertexAttributeSemantic semantic);
    DXGI_FORMAT ToVertexAttributeDataType(VertexAttributeDataType dataType, int dimensions, bool normalized);
    DXGI_FORMAT ToTextureInternalFormat(TextureInternalFormat format, bool isLinear);
    D3D12_BLEND ToBlendFactor(BlendFactor factor);
    D3D12_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(PrimitiveType primitiveType);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE ToPrimitiveTopologyType(PrimitiveType primitiveType);
    D3D12_SRV_DIMENSION ToResourceViewDimension(TextureType textureType);
    D3D12_RTV_DIMENSION ToColorTargetViewDimension(TextureType textureType);
    D3D12_DSV_DIMENSION ToDepthTargetViewDimension(TextureType textureType);
    D3D12_FILTER ToTextureFilterMode(TextureFilteringMode filteringMode);
    D3D12_TEXTURE_ADDRESS_MODE ToTextureWrapMode(TextureWrapMode wrapMode);
    D3D12_CULL_MODE ToCullFace(CullFace face);
    D3D12_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction function);
    D3D12_RESOURCE_STATES ToResourceState(ResourceState state);
}

#endif

#endif //RENDER_ENGINE_DX12_HELPERS_H
