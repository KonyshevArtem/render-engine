#include "texture.h"
#include "graphics_backend_api.h"

Texture::Texture(TextureType textureType, TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, bool isLinear, bool isRenderTarget) :
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
    m_Texture = GraphicsBackend::Current()->CreateTexture(m_Width, m_Height, m_Depth, m_TextureType, format, mipLevels, isLinear, isRenderTarget);
    RecreateSampler(false);
}

Texture::~Texture()
{
    GraphicsBackend::Current()->DeleteTexture(m_Texture);
    GraphicsBackend::Current()->DeleteSampler(m_Sampler);
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

void Texture::UploadPixels(void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    GraphicsBackend::Current()->UploadImagePixels(m_Texture, mipLevel, cubemapFace, width, height, depth, size, pixels);
}

void Texture::RecreateSampler(bool deleteOld)
{
    if (deleteOld)
    {
        GraphicsBackend::Current()->DeleteSampler(m_Sampler);
    }

    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x, m_MinLod);
}
