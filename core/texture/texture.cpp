#include "texture.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"

Texture::Texture(TextureType textureType, const GraphicsBackendTextureDescriptor& descriptor, const std::string& name) :
        m_TextureDescriptor(descriptor),
		m_TextureType(textureType),
		m_SamplerName(name + "_Sampler"),
		m_SamplerDescriptor({}),
		m_SamplerDirty(true),
		m_HasSampler(false)
{
    Profiler::Marker _("Texture::Texture");

    m_SamplerDescriptor.WrapMode = TextureWrapMode::REPEAT;
    m_SamplerDescriptor.FilteringMode = descriptor.MipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR;
    m_SamplerDescriptor.HasBorderColor = true;
    m_Texture = GraphicsBackend::Current()->CreateTexture(m_TextureType, m_TextureDescriptor, name);
}

Texture::~Texture()
{
    GraphicsBackend::Current()->DeleteTexture(m_Texture);
    if (m_HasSampler)
		GraphicsBackend::Current()->DeleteSampler(m_Sampler);
}

void Texture::SetMinMipLevel(int minMipLevel)
{
    m_SamplerDescriptor.MinLod = minMipLevel;
    m_SamplerDirty = true;
}

void Texture::SetWrapMode(TextureWrapMode wrapMode)
{
    m_SamplerDescriptor.WrapMode = wrapMode;
    m_SamplerDirty = true;
}

void Texture::SetBorderColor(const Vector4& color)
{
    memcpy(&m_SamplerDescriptor.BorderColor[0], &color, sizeof(Vector4));
    m_SamplerDirty = true;
}

void Texture::SetFilteringMode(TextureFilteringMode mode)
{
    m_SamplerDescriptor.FilteringMode = mode;
    m_SamplerDirty = true;
}

void Texture::SetComparisonFunction(ComparisonFunction function)
{
    m_SamplerDescriptor.ComparisonFunction = function;
    m_SamplerDirty = true;
}

const GraphicsBackendSampler& Texture::GetBackendSampler()
{
    if (m_SamplerDirty)
        RecreateSampler();
    return m_Sampler;
}

void Texture::UploadPixels(const void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace) const
{
    Profiler::Marker _("Texture::UploadPixels");

    const int sizeMultiplier = 1 << mipLevel;
    const unsigned int width = m_TextureDescriptor.Width / sizeMultiplier;
    const unsigned int height = m_TextureDescriptor.Height / sizeMultiplier;

    GraphicsBackend::Current()->UploadImagePixels(GetBackendTexture(), mipLevel, cubemapFace, width, height, depth, size, pixels);
}

void Texture::RecreateSampler()
{
    if (m_HasSampler)
        GraphicsBackend::Current()->DeleteSampler(m_Sampler);

    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_SamplerDescriptor, m_SamplerName);
    m_HasSampler = true;
    m_SamplerDirty = false;
}
