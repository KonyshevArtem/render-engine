#ifndef RENDER_ENGINE_METAL_HELPERS_H
#define RENDER_ENGINE_METAL_HELPERS_H

#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"

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
}

#endif //RENDER_ENGINE_METAL_HELPERS_H
