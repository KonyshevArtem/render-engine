#include "texture.h"
#include "graphics_backend_api.h"

Texture::Texture(TextureType textureType, TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        m_TextureType(textureType),
        m_Width(width),
        m_Height(height),
        m_Depth(depth),
        m_MipLevels(mipLevels),
        m_WrapMode(TextureWrapMode::REPEAT),
        m_FilteringMode(mipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR),
        m_BorderColor(Vector4::Zero()),
        m_MinLod(0)
{
    m_Texture = GraphicsBackend::Current()->CreateTexture(m_Width, m_Height, m_TextureType, format, mipLevels);
    RecreateSampler(false);
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

void Texture::SetMinMipLevel(int minMipLevel)
{
    m_MinLod = minMipLevel;
    RecreateSampler(true);
}

void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    m_WrapMode = wrapMode;
    RecreateSampler(true);
}

void Texture::SetBorderColor(const Vector4 &color)
{
    m_BorderColor = color;
    RecreateSampler(true);
}

void Texture::UploadPixels(void *pixels, int size, int depth, int mipLevel, int slice) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    GraphicsBackend::Current()->UploadImagePixels(m_Texture, mipLevel, slice, width, height, depth, size, pixels);
}

void Texture::RecreateSampler(bool deleteOld)
{
    if (deleteOld)
    {
        GraphicsBackend::Current()->DeleteSampler(m_Sampler);
    }

    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x, m_MinLod);
}
