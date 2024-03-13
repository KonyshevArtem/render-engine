#ifndef RENDER_ENGINE_TEXTURE_INTERNAL_FORMAT_H
#define RENDER_ENGINE_TEXTURE_INTERNAL_FORMAT_H

#include "graphics_backend.h"

enum class TextureInternalFormat : GRAPHICS_BACKEND_TYPE_ENUM
{
    RED         = GRAPHICS_BACKEND_RED,
    RG          = GRAPHICS_BACKEND_RG,
    RGB         = GRAPHICS_BACKEND_RGB,
    RGBA        = GRAPHICS_BACKEND_RGBA,
    SRGB        = GRAPHICS_BACKEND_SRGB,
    SRGB_ALPHA  = GRAPHICS_BACKEND_SRGB_ALPHA,

    COMPRESSED_RGB          = GRAPHICS_BACKEND_COMPRESSED_RGB,
    COMPRESSED_RGBA         = GRAPHICS_BACKEND_COMPRESSED_RGBA,
    COMPRESSED_SRGB         = GRAPHICS_BACKEND_COMPRESSED_SRGB,
    COMPRESSED_SRGB_ALPHA   = GRAPHICS_BACKEND_COMPRESSED_SRGB_ALPHA,
    COMPRESSED_RED          = GRAPHICS_BACKEND_COMPRESSED_RED,
    COMPRESSED_RG           = GRAPHICS_BACKEND_COMPRESSED_RG,

    R8              = GRAPHICS_BACKEND_R8,
    R8_SNORM        = GRAPHICS_BACKEND_R8_SNORM,
    R16             = GRAPHICS_BACKEND_R16,
    R16_SNORM       = GRAPHICS_BACKEND_R16_SNORM,
    RG8             = GRAPHICS_BACKEND_RG8,
    RG8_SNORM       = GRAPHICS_BACKEND_RG8_SNORM,
    RG16            = GRAPHICS_BACKEND_RG16,
    RG16_SNORM      = GRAPHICS_BACKEND_RG16_SNORM,
    R3_G3_B2        = GRAPHICS_BACKEND_R3_G3_B2,
    RGB4            = GRAPHICS_BACKEND_RGB4,
    RGB5            = GRAPHICS_BACKEND_RGB5,
    RGB8            = GRAPHICS_BACKEND_RGB8,
    RGB8_SNORM      = GRAPHICS_BACKEND_RGB8_SNORM,
    RGB10           = GRAPHICS_BACKEND_RGB10,
    RGB12           = GRAPHICS_BACKEND_RGB12,
    RGB16           = GRAPHICS_BACKEND_RGB16,
    RGBA2           = GRAPHICS_BACKEND_RGBA2,
    RGBA4           = GRAPHICS_BACKEND_RGBA4,
    RGB5_A1         = GRAPHICS_BACKEND_RGB5_A1,
    RGBA8           = GRAPHICS_BACKEND_RGBA8,
    RGBA8_SNORM     = GRAPHICS_BACKEND_RGBA8_SNORM,
    RGB10_A2        = GRAPHICS_BACKEND_RGB10_A2,
    RGB10_A2UI      = GRAPHICS_BACKEND_RGB10_A2UI,
    RGBA12          = GRAPHICS_BACKEND_RGBA12,
    RGBA16          = GRAPHICS_BACKEND_RGBA16,
    SRGB8           = GRAPHICS_BACKEND_SRGB8,
    SRGB8_ALPHA8    = GRAPHICS_BACKEND_SRGB8_ALPHA8,
    R16F            = GRAPHICS_BACKEND_R16F,
    RG16F           = GRAPHICS_BACKEND_RG16F,
    RGB16F          = GRAPHICS_BACKEND_RGB16F,
    RGBA16F         = GRAPHICS_BACKEND_RGBA16F,
    R32F            = GRAPHICS_BACKEND_R32F,
    RG32F           = GRAPHICS_BACKEND_RG32F,
    RGB32F          = GRAPHICS_BACKEND_RGB32F,
    RGBA32F         = GRAPHICS_BACKEND_RGBA32F,
    R11F_G11F_B10F  = GRAPHICS_BACKEND_R11F_G11F_B10F,
    RGB9_E5         = GRAPHICS_BACKEND_RGB9_E5,
    R8I             = GRAPHICS_BACKEND_R8I,
    R8UI            = GRAPHICS_BACKEND_R8UI,
    R16I            = GRAPHICS_BACKEND_R16I,
    R16UI           = GRAPHICS_BACKEND_R16UI,
    R32I            = GRAPHICS_BACKEND_R32I,
    R32UI           = GRAPHICS_BACKEND_R32UI,
    RG8I            = GRAPHICS_BACKEND_RG8I,
    RG8UI           = GRAPHICS_BACKEND_RG8UI,
    RG16I           = GRAPHICS_BACKEND_RG16I,
    RG16UI          = GRAPHICS_BACKEND_RG16UI,
    RG32I           = GRAPHICS_BACKEND_RG32I,
    RG32UI          = GRAPHICS_BACKEND_RG32UI,
    RGB8I           = GRAPHICS_BACKEND_RGB8I,
    RGB8UI          = GRAPHICS_BACKEND_RGB8UI,
    RGB16I          = GRAPHICS_BACKEND_RGB16I,
    RGB16UI         = GRAPHICS_BACKEND_RGB16UI,
    RGB32I          = GRAPHICS_BACKEND_RGB32I,
    RGB32UI         = GRAPHICS_BACKEND_RGB32UI,
    RGBA8I          = GRAPHICS_BACKEND_RGBA8I,
    RGBA8UI         = GRAPHICS_BACKEND_RGBA8UI,
    RGBA16I         = GRAPHICS_BACKEND_RGBA16I,
    RGBA16UI        = GRAPHICS_BACKEND_RGBA16UI,
    RGBA32I         = GRAPHICS_BACKEND_RGBA32I,
    RGBA32UI        = GRAPHICS_BACKEND_RGBA32UI,

    COMPRESSED_RGB_S3TC_DXT1_EXT  = GRAPHICS_BACKEND_COMPRESSED_RGB_S3TC_DXT1_EXT,
    COMPRESSED_RGBA_S3TC_DXT1_EXT = GRAPHICS_BACKEND_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    COMPRESSED_RGBA_S3TC_DXT3_EXT = GRAPHICS_BACKEND_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    COMPRESSED_RGBA_S3TC_DXT5_EXT = GRAPHICS_BACKEND_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    COMPRESSED_SRGB_S3TC_DXT1_EXT       = GRAPHICS_BACKEND_COMPRESSED_SRGB_S3TC_DXT1_EXT,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = GRAPHICS_BACKEND_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT = GRAPHICS_BACKEND_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT = GRAPHICS_BACKEND_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

    COMPRESSED_RED_RGTC1            = GRAPHICS_BACKEND_COMPRESSED_RED_RGTC1,
    COMPRESSED_SIGNED_RED_RGTC1     = GRAPHICS_BACKEND_COMPRESSED_SIGNED_RED_RGTC1,
    COMPRESSED_RG_RGTC2             = GRAPHICS_BACKEND_COMPRESSED_RG_RGTC2,
    COMPRESSED_SIGNED_RG_RGTC2      = GRAPHICS_BACKEND_COMPRESSED_SIGNED_RG_RGTC2,

    DEPTH_COMPONENT = GRAPHICS_BACKEND_DEPTH_COMPONENT,
    DEPTH_STENCIL   = GRAPHICS_BACKEND_DEPTH_STENCIL,
};

#endif //RENDER_ENGINE_TEXTURE_INTERNAL_FORMAT_H
