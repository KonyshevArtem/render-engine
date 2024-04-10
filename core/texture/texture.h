#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include "enums/texture_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/framebuffer_attachment.h"
#include "enums/framebuffer_target.h"
#include "enums/texture_target.h"
#include "enums/texture_internal_format.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_resource_bindings.h"
#include "vector4/vector4.h"

class Texture
{
public:
    virtual ~Texture();

    void Bind(const GraphicsBackendResourceBindings &bindings, bool bindSampler, int uniformLocation) const;
    void Attach(FramebufferTarget target, FramebufferAttachment attachment, int level, int layer) const;
    void SetBaseMipLevel(unsigned int baseMipLevel) const;
    void SetWrapMode(TextureWrapMode wrapMode);
    void SetBorderColor(const Vector4 &color);

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
    Texture(TextureType textureType, TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels);

    void UploadPixels(void *pixels, int size, int depth, int mipLevel, int slice) const;

private:
    unsigned int m_Width = 0;
    unsigned int m_Height = 0;
    unsigned int m_Depth = 0;
    GraphicsBackendTexture m_Texture{};
    GraphicsBackendSampler m_Sampler{};
    TextureType m_TextureType = TextureType::TEXTURE_2D;
    unsigned int m_MipLevels = 0;

    TextureWrapMode m_WrapMode;
    TextureFilteringMode m_FilteringMode;
    Vector4 m_BorderColor;
};

#endif //RENDER_ENGINE_TEXTURE_H
