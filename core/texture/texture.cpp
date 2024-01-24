#include "texture.h"
#include "vector4/vector4.h"
#include "graphics_backend_api.h"
#include "enums/texture_parameter.h"
#include "enums/framebuffer_target.h"
#include "enums/sampler_parameter.h"

Texture::Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        m_TextureType(textureType),
        m_Width(width),
        m_Height(height),
        m_Depth(depth),
        m_MipLevels(mipLevels)
{
    GraphicsBackend::GenerateTextures(1, &m_Texture);
    GraphicsBackend::GenerateSampler(1, &m_Sampler);
    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    SetWrapMode_Internal(TextureWrapMode::REPEAT);
    SetFiltering_Internal(mipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR);
    GraphicsBackend::SetTextureParameterInt(m_TextureType, TextureParameter::MAX_LEVEL, mipLevels - 1);
    GraphicsBackend::BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

Texture::~Texture()
{
    GraphicsBackend::DeleteTextures(1, &m_Texture);
    GraphicsBackend::DeleteSamplers(1, &m_Sampler);
}

void Texture::Bind(TextureUnit unit) const
{
    GraphicsBackend::SetActiveTextureUnit(unit);
    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::BindSampler(unit, m_Sampler);
}

void Texture::Attach(FramebufferAttachment attachment, int level, int layer) const
{
    if (m_TextureType == TextureType::TEXTURE_2D)
    {
        GraphicsBackend::SetFramebufferTexture(FramebufferTarget::FRAMEBUFFER, attachment, m_Texture, level);
    }
    else
    {
        GraphicsBackend::SetFramebufferTextureLayer(FramebufferTarget::FRAMEBUFFER, attachment, m_Texture, level, layer);
    }
}

void Texture::SetBaseMipLevel(unsigned int baseMipLevel) const
{
    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::SetTextureParameterInt(m_TextureType, TextureParameter::BASE_LEVEL, baseMipLevel);
    GraphicsBackend::BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetWrapMode(TextureWrapMode wrapMode) const
{
    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    SetWrapMode_Internal(wrapMode);
    GraphicsBackend::BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetWrapMode_Internal(TextureWrapMode wrapMode) const
{
    GraphicsBackend::SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_S, static_cast<int>(wrapMode));
    GraphicsBackend::SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_T, static_cast<int>(wrapMode));
    GraphicsBackend::SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_R, static_cast<int>(wrapMode));
}

void Texture::SetBorderColor(const Vector4 &color) const
{
    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::SetSamplerParameterFloatArray(m_Sampler, SamplerParameter::BORDER_COLOR, &color.x);
    GraphicsBackend::BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetFiltering_Internal(TextureFilteringMode filtering) const
{
    GraphicsBackend::SetSamplerParameterInt(m_Sampler, SamplerParameter::MIN_FILTER, static_cast<int>(filtering));

    if (filtering == TextureFilteringMode::LINEAR_MIPMAP_NEAREST || filtering == TextureFilteringMode::LINEAR_MIPMAP_LINEAR)
    {
        filtering = TextureFilteringMode::LINEAR;
    }
    else if (filtering == TextureFilteringMode::NEAREST_MIPMAP_NEAREST || filtering == TextureFilteringMode::NEAREST_MIPMAP_LINEAR)
    {
        filtering = TextureFilteringMode::NEAREST;
    }
    GraphicsBackend::SetSamplerParameterInt(m_Sampler, SamplerParameter::MAG_FILTER, static_cast<int>(filtering));
}

void Texture::UploadPixels(void *pixels, TextureTarget target, TextureInternalFormat textureFormat, TexturePixelFormat pixelFormat, TextureDataType dataType, int size, int mipLevel, bool compressed) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    bool isImage3D = m_TextureType == TextureType::TEXTURE_2D_ARRAY;

    GraphicsBackend::BindTexture(m_TextureType, m_Texture);
    if (compressed)
    {
        if (isImage3D)
        {
            GraphicsBackend::TextureCompressedImage3D(target, mipLevel, textureFormat, width, height, m_Depth, 0, size, pixels);
        }
        else
        {
            GraphicsBackend::TextureCompressedImage2D(target, mipLevel, textureFormat, width, height, 0, size, pixels);
        }
    }
    else
    {
        if (isImage3D)
        {
            GraphicsBackend::TextureImage3D(target, mipLevel, textureFormat, width, height, m_Depth, 0, pixelFormat, dataType, pixels);
        }
        else
        {
            GraphicsBackend::TextureImage2D(target, mipLevel, textureFormat, width, height, 0, pixelFormat, dataType, pixels);
        }
    }
    GraphicsBackend::BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}