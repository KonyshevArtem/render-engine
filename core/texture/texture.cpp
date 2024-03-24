#include "texture.h"
#include "vector4/vector4.h"
#include "graphics_backend_api.h"
#include "enums/texture_parameter.h"
#include "enums/sampler_parameter.h"

Texture::Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        m_TextureType(textureType),
        m_Width(width),
        m_Height(height),
        m_Depth(depth),
        m_MipLevels(mipLevels)
{
    GraphicsBackend::Current()->GenerateTextures(1, &m_Texture);
    GraphicsBackend::Current()->GenerateSampler(1, &m_Sampler);
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    SetWrapMode_Internal(TextureWrapMode::REPEAT);
    SetFiltering_Internal(mipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR);
    GraphicsBackend::Current()->SetTextureParameterInt(m_TextureType, TextureParameter::MAX_LEVEL, mipLevels - 1);
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

Texture::~Texture()
{
    GraphicsBackend::Current()->DeleteTextures(1, &m_Texture);
    GraphicsBackend::Current()->DeleteSamplers(1, &m_Sampler);
}

void Texture::Bind(TextureUnit unit) const
{
    GraphicsBackend::Current()->SetActiveTextureUnit(unit);
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::Current()->BindSampler(unit, m_Sampler);
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

void Texture::SetWrapMode(TextureWrapMode wrapMode) const
{
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    SetWrapMode_Internal(wrapMode);
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetWrapMode_Internal(TextureWrapMode wrapMode) const
{
    GraphicsBackend::Current()->SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_S, static_cast<int>(wrapMode));
    GraphicsBackend::Current()->SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_T, static_cast<int>(wrapMode));
    GraphicsBackend::Current()->SetSamplerParameterInt(m_Sampler, SamplerParameter::WRAP_R, static_cast<int>(wrapMode));
}

void Texture::SetBorderColor(const Vector4 &color) const
{
    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    GraphicsBackend::Current()->SetSamplerParameterFloatArray(m_Sampler, SamplerParameter::BORDER_COLOR, &color.x);
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}

void Texture::SetFiltering_Internal(TextureFilteringMode filtering) const
{
    GraphicsBackend::Current()->SetSamplerParameterInt(m_Sampler, SamplerParameter::MIN_FILTER, static_cast<int>(filtering));

    if (filtering == TextureFilteringMode::LINEAR_MIPMAP_NEAREST || filtering == TextureFilteringMode::LINEAR_MIPMAP_LINEAR)
    {
        filtering = TextureFilteringMode::LINEAR;
    }
    else if (filtering == TextureFilteringMode::NEAREST_MIPMAP_NEAREST || filtering == TextureFilteringMode::NEAREST_MIPMAP_LINEAR)
    {
        filtering = TextureFilteringMode::NEAREST;
    }
    GraphicsBackend::Current()->SetSamplerParameterInt(m_Sampler, SamplerParameter::MAG_FILTER, static_cast<int>(filtering));
}

void Texture::UploadPixels(void *pixels, TextureTarget target, TextureInternalFormat textureFormat, TexturePixelFormat pixelFormat, TextureDataType dataType, int size, int mipLevel, bool compressed) const
{
    int sizeMultiplier = 1 << mipLevel;
    unsigned int width = m_Width / sizeMultiplier;
    unsigned int height = m_Height / sizeMultiplier;

    bool isImage3D = m_TextureType == TextureType::TEXTURE_2D_ARRAY;

    GraphicsBackend::Current()->BindTexture(m_TextureType, m_Texture);
    if (compressed)
    {
        if (isImage3D)
        {
            GraphicsBackend::Current()->TextureCompressedImage3D(target, mipLevel, textureFormat, width, height, m_Depth, 0, size, pixels);
        }
        else
        {
            GraphicsBackend::Current()->TextureCompressedImage2D(target, mipLevel, textureFormat, width, height, 0, size, pixels);
        }
    }
    else
    {
        if (isImage3D)
        {
            GraphicsBackend::Current()->TextureImage3D(target, mipLevel, textureFormat, width, height, m_Depth, 0, pixelFormat, dataType, pixels);
        }
        else
        {
            GraphicsBackend::Current()->TextureImage2D(target, mipLevel, textureFormat, width, height, 0, pixelFormat, dataType, pixels);
        }
    }
    GraphicsBackend::Current()->BindTexture(m_TextureType, GraphicsBackendTexture::NONE);
}