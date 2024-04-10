#include "texture.h"
#include "graphics_backend_api.h"
#include "enums/texture_parameter.h"

Texture::Texture(TextureType textureType, TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        m_TextureType(textureType),
        m_Width(width),
        m_Height(height),
        m_Depth(depth),
        m_MipLevels(mipLevels),
        m_WrapMode(TextureWrapMode::REPEAT),
        m_FilteringMode(mipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR),
        m_BorderColor(Vector4::Zero())
{
    m_Texture = GraphicsBackend::Current()->CreateTexture(m_Width, m_Height, m_TextureType, format);
    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x);
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::Current()->SetTextureParameterInt(m_TextureType, TextureParameter::MAX_LEVEL, mipLevels - 1);
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

Texture::~Texture()
{
    GraphicsBackend::Current()->DeleteTexture(m_Texture);
    GraphicsBackend::Current()->DeleteSampler(m_Sampler);
}

void Texture::Bind(const GraphicsBackendResourceBindings &bindings, bool bindSampler, int uniformLocation) const
{
    GraphicsBackend::Current()->BindTexture(bindings, uniformLocation, m_Texture);
    if (bindSampler)
    {
        GraphicsBackend::Current()->BindSampler(bindings, m_Sampler);
    }
}

void Texture::Attach(FramebufferTarget target, FramebufferAttachment attachment, int level, int layer) const
{
    if (m_TextureType == TextureType::TEXTURE_2D)
    {
        GraphicsBackend::Current()->SetFramebufferTexture(target, attachment, m_Texture, level);
    }
    else
    {
        GraphicsBackend::Current()->SetFramebufferTextureLayer(target, attachment, m_Texture, level, layer);
    }
}

void Texture::SetBaseMipLevel(unsigned int baseMipLevel) const
{
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::Current()->SetTextureParameterInt(m_TextureType, TextureParameter::BASE_LEVEL, baseMipLevel);
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    GraphicsBackend::Current()->DeleteSampler(m_Sampler);

    m_WrapMode = wrapMode;
    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x);
}

void Texture::SetBorderColor(const Vector4 &color)
{
    GraphicsBackend::Current()->DeleteSampler(m_Sampler);

    m_BorderColor = color;
    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x);
}

void Texture::UploadPixels(void *pixels, int size, int depth, int mipLevel, int slice) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    GraphicsBackend::Current()->UploadImagePixels(m_Texture, mipLevel, slice, width, height, depth, size, pixels);
}