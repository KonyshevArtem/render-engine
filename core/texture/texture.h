#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include "enums/texture_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/framebuffer_attachment.h"
#include "enums/framebuffer_target.h"
#include "enums/texture_target.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_pixel_format.h"
#include "enums/texture_data_type.h"
#include "enums/texture_unit.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"

struct Vector4;

class Texture
{
public:
    virtual ~Texture();

    void Bind(TextureUnit unit) const;
    void Attach(FramebufferTarget target, FramebufferAttachment attachment, int level, int layer) const;
    void SetBaseMipLevel(unsigned int baseMipLevel) const;
    void SetWrapMode(TextureWrapMode wrapMode) const;
    void SetBorderColor(const Vector4 &color) const;

    inline unsigned int GetWidth() const
    {
        return m_Width;
    }

    inline unsigned int GetHeight() const
    {
        return m_Height;
    }

    inline unsigned int GetMipLevels() const
    {
        return m_MipLevels;
    }

    Texture(const Texture &) = delete;
    Texture(Texture &&) = delete;

    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&) = delete;

protected:
    Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels);

    void UploadPixels(void *pixels, TextureTarget target, TextureInternalFormat textureFormat, TexturePixelFormat pixelFormat, TextureDataType dataType, int size, int mipLevel, bool compressed) const;

private:
    void SetWrapMode_Internal(TextureWrapMode wrapMode) const;
    void SetFiltering_Internal(TextureFilteringMode filtering) const;

    unsigned int m_Width = 0;
    unsigned int m_Height = 0;
    unsigned int m_Depth = 0;
    GraphicsBackendTexture m_Texture{};
    GraphicsBackendSampler m_Sampler{};
    TextureType m_TextureType = TextureType::TEXTURE_2D;
    unsigned int m_MipLevels = 0;
};

#endif //RENDER_ENGINE_TEXTURE_H
