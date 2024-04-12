#include "graphics_backend_api_base.h"
#include "graphics_backend_api_opengl.h"
#include "graphics_backend_api_metal.h"
#include "enums/texture_internal_format.h"

GraphicsBackendBase *GraphicsBackendBase::Create(const std::string &backend)
{
    if (backend == "OpenGL")
    {
        return new GraphicsBackendOpenGL();
    }

#if RENDER_ENGINE_APPLE
    if (backend == "Metal")
    {
        return new GraphicsBackendMetal();
    }
#endif

    return nullptr;
}

bool GraphicsBackendBase::IsCompressedTextureFormat(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::RED:
        case TextureInternalFormat::RG:
        case TextureInternalFormat::RGB:
        case TextureInternalFormat::RGBA:
        case TextureInternalFormat::SRGB:
        case TextureInternalFormat::SRGB_ALPHA:
        case TextureInternalFormat::R8:
        case TextureInternalFormat::R8_SNORM:
        case TextureInternalFormat::R16:
        case TextureInternalFormat::R16_SNORM:
        case TextureInternalFormat::RG8:
        case TextureInternalFormat::RG8_SNORM:
        case TextureInternalFormat::RG16:
        case TextureInternalFormat::RG16_SNORM:
        case TextureInternalFormat::R3_G3_B2:
        case TextureInternalFormat::RGB4:
        case TextureInternalFormat::RGB5:
        case TextureInternalFormat::RGB8:
        case TextureInternalFormat::RGB8_SNORM:
        case TextureInternalFormat::RGB10:
        case TextureInternalFormat::RGB12:
        case TextureInternalFormat::RGB16:
        case TextureInternalFormat::RGBA2:
        case TextureInternalFormat::RGBA4:
        case TextureInternalFormat::RGB5_A1:
        case TextureInternalFormat::RGBA8:
        case TextureInternalFormat::RGBA8_SNORM:
        case TextureInternalFormat::RGB10_A2:
        case TextureInternalFormat::RGB10_A2UI:
        case TextureInternalFormat::RGBA12:
        case TextureInternalFormat::RGBA16:
        case TextureInternalFormat::SRGB8:
        case TextureInternalFormat::SRGB8_ALPHA8:
        case TextureInternalFormat::R16F:
        case TextureInternalFormat::RG16F:
        case TextureInternalFormat::RGB16F:
        case TextureInternalFormat::RGBA16F:
        case TextureInternalFormat::R32F:
        case TextureInternalFormat::RG32F:
        case TextureInternalFormat::RGB32F:
        case TextureInternalFormat::RGBA32F:
        case TextureInternalFormat::R11F_G11F_B10F:
        case TextureInternalFormat::RGB9_E5:
        case TextureInternalFormat::R8I:
        case TextureInternalFormat::R8UI:
        case TextureInternalFormat::R16I:
        case TextureInternalFormat::R16UI:
        case TextureInternalFormat::R32I:
        case TextureInternalFormat::R32UI:
        case TextureInternalFormat::RG8I:
        case TextureInternalFormat::RG8UI:
        case TextureInternalFormat::RG16I:
        case TextureInternalFormat::RG16UI:
        case TextureInternalFormat::RG32I:
        case TextureInternalFormat::RG32UI:
        case TextureInternalFormat::RGB8I:
        case TextureInternalFormat::RGB8UI:
        case TextureInternalFormat::RGB16I:
        case TextureInternalFormat::RGB16UI:
        case TextureInternalFormat::RGB32I:
        case TextureInternalFormat::RGB32UI:
        case TextureInternalFormat::RGBA8I:
        case TextureInternalFormat::RGBA8UI:
        case TextureInternalFormat::RGBA16I:
        case TextureInternalFormat::RGBA16UI:
        case TextureInternalFormat::RGBA32I:
        case TextureInternalFormat::RGBA32UI:
        case TextureInternalFormat::DEPTH_COMPONENT:
        case TextureInternalFormat::DEPTH_STENCIL:
            return false;
        case TextureInternalFormat::COMPRESSED_RGB:
        case TextureInternalFormat::COMPRESSED_RGBA:
        case TextureInternalFormat::COMPRESSED_SRGB:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA:
        case TextureInternalFormat::COMPRESSED_RED:
        case TextureInternalFormat::COMPRESSED_RG:
        case TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
        case TextureInternalFormat::COMPRESSED_RED_RGTC1:
        case TextureInternalFormat::COMPRESSED_SIGNED_RED_RGTC1:
        case TextureInternalFormat::COMPRESSED_RG_RGTC2:
        case TextureInternalFormat::COMPRESSED_SIGNED_RG_RGTC2:
            return true;
    }
}

int GraphicsBackendBase::GetBlockSize(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return 4;
        default:
            return 0;
    }
}

int GraphicsBackendBase::GetBlockBytes(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::COMPRESSED_RGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_S3TC_DXT1_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return 8;
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
        case TextureInternalFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case TextureInternalFormat::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return 16;
        default:
            return 0;
    }
}
