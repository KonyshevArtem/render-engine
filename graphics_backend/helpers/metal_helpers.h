#ifndef RENDER_ENGINE_METAL_HELPERS_H
#define RENDER_ENGINE_METAL_HELPERS_H

#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/load_action.h"
#include "enums/store_action.h"
#include "enums/depth_function.h"
#include "enums/cull_face_orientation.h"
#include "enums/cull_face.h"
#include "enums/blend_factor.h"

#include <cstdint>

namespace MTL
{
    enum PrimitiveType : std::uintptr_t;
    enum IndexType : std::uintptr_t;
    enum TextureType : std::uintptr_t;
    enum PixelFormat : std::uintptr_t;
    enum SamplerAddressMode : std::uintptr_t;
    enum SamplerMinMagFilter : std::uintptr_t;
    enum SamplerBorderColor : std::uintptr_t;
    enum VertexFormat : std::uintptr_t;
    enum LoadAction : std::uintptr_t;
    enum StoreAction : std::uintptr_t;
    enum CompareFunction : std::uintptr_t;
    enum Winding : std::uintptr_t;
    enum CullMode : std::uintptr_t;
    enum BlendFactor : std::uintptr_t;
}

namespace MetalHelpers
{
    MTL::PrimitiveType ToPrimitiveType(PrimitiveType primitiveType);
    MTL::IndexType ToIndicesDataType(IndicesDataType dataType);
    MTL::TextureType ToTextureType(TextureType textureType);
    MTL::PixelFormat ToTextureInternalFormat(TextureInternalFormat format);
    MTL::SamplerAddressMode ToTextureWrapMode(TextureWrapMode wrapMode);
    MTL::SamplerMinMagFilter ToTextureFilteringMode(TextureFilteringMode filteringMode);
    MTL::SamplerBorderColor ToTextureBorderColor(const float color[4]);
    MTL::VertexFormat ToVertexFormat(VertexAttributeDataType dataType, int dimensions, bool normalized);
    MTL::LoadAction ToLoadAction(LoadAction loadAction);
    MTL::StoreAction ToStoreAction(StoreAction storeAction);
    MTL::CompareFunction ToDepthCompareFunction(DepthFunction function);
    MTL::Winding ToCullFaceOrientation(CullFaceOrientation orientation);
    MTL::CullMode ToCullFace(CullFace face);
    MTL::BlendFactor ToBlendFactor(BlendFactor factor);
}

#endif //RENDER_ENGINE_METAL_HELPERS_H
