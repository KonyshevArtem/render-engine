#include "graphics_backend_api_base.h"
#include "graphics_backend_api_opengl.h"
#include "graphics_backend_api_metal.h"
#include "graphics_backend_api_dx12.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_type.h"
#include "enums/framebuffer_attachment.h"
#include "arguments.h"

GraphicsBackendBase *GraphicsBackendBase::Create()
{
    const bool openGL = Arguments::Contains("-opengl");
    const bool dx12 = Arguments::Contains("-dx12");
    const bool metal = Arguments::Contains("-metal");

#ifdef RENDER_BACKEND_OPENGL
    if (openGL && !dx12)
    {
        return new GraphicsBackendOpenGL();
    }
#endif

#ifdef RENDER_BACKEND_METAL
    if (metal)
    {
        return new GraphicsBackendMetal();
    }
#endif

#ifdef RENDER_BACKEND_DX12
    return new GraphicsBackendDX12();
#endif

    return nullptr;
}

void GraphicsBackendBase::InitNewFrame()
{
    ++m_FrameCount;
}

uint64_t GraphicsBackendBase::GetFrameNumber() const
{
    return m_FrameCount;
}

bool GraphicsBackendBase::IsTexture3D(TextureType type)
{
    return type == TextureType::TEXTURE_2D_ARRAY || type == TextureType::TEXTURE_3D || type == TextureType::TEXTURE_CUBEMAP_ARRAY;
}

bool GraphicsBackendBase::IsCompressedTextureFormat(TextureInternalFormat format)
{
    switch (format)
    {
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
        case TextureInternalFormat::DEPTH_32:
        case TextureInternalFormat::DEPTH_24:
        case TextureInternalFormat::DEPTH_16:
        case TextureInternalFormat::DEPTH_32_STENCIL_8:
        case TextureInternalFormat::DEPTH_24_STENCIL_8:
            return false;
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC4:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
        case TextureInternalFormat::ASTC_5X5:
        case TextureInternalFormat::ASTC_6X6:
        case TextureInternalFormat::ASTC_8X8:
        case TextureInternalFormat::ASTC_10X10:
        case TextureInternalFormat::ASTC_12X12:
            return true;
    }
}

int GraphicsBackendBase::GetBlockSize(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC4:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
            return 4;
        case TextureInternalFormat::ASTC_5X5:
            return 5;
        case TextureInternalFormat::ASTC_6X6:
            return 6;
        case TextureInternalFormat::ASTC_8X8:
            return 8;
        case TextureInternalFormat::ASTC_10X10:
            return 10;
        case TextureInternalFormat::ASTC_12X12:
            return 12;
        default:
            return 0;
    }
}

int GraphicsBackendBase::GetBlockBytes(TextureInternalFormat format)
{
    switch (format)
    {
        case TextureInternalFormat::BC1_RGB:
        case TextureInternalFormat::BC1_RGBA:
        case TextureInternalFormat::BC4:
            return 8;
        case TextureInternalFormat::BC2:
        case TextureInternalFormat::BC3:
        case TextureInternalFormat::BC5:
        case TextureInternalFormat::BC6H:
        case TextureInternalFormat::BC7:
        case TextureInternalFormat::ASTC_4X4:
        case TextureInternalFormat::ASTC_5X5:
        case TextureInternalFormat::ASTC_6X6:
        case TextureInternalFormat::ASTC_8X8:
        case TextureInternalFormat::ASTC_10X10:
        case TextureInternalFormat::ASTC_12X12:
            return 16;
        default:
            return 0;
    }
}

bool GraphicsBackendBase::IsDepthFormat(TextureInternalFormat format)
{
    return format == TextureInternalFormat::DEPTH_16 ||
            format == TextureInternalFormat::DEPTH_24 ||
            format == TextureInternalFormat::DEPTH_32 ||
            format == TextureInternalFormat::DEPTH_24_STENCIL_8 ||
            format == TextureInternalFormat::DEPTH_32_STENCIL_8;
}

bool GraphicsBackendBase::IsDepthAttachment(FramebufferAttachment attachment)
{
    return attachment == FramebufferAttachment::DEPTH_ATTACHMENT ||
            attachment == FramebufferAttachment::STENCIL_ATTACHMENT ||
            attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
}
