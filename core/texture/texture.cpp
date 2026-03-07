#include "texture.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"

Texture::Texture(TextureType textureType, const Descriptor& descriptor, const std::string& name) :
        m_Width(descriptor.Width),
        m_Height(descriptor.Height),
        m_Depth(descriptor.Depth),
		m_TextureType(textureType),
        m_MipLevels(descriptor.MipLevels),
		m_SamplerName(name + "_Sampler"),
        m_WrapMode(TextureWrapMode::REPEAT),
        m_FilteringMode(descriptor.MipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR),
        m_ComparisonFunction(ComparisonFunction::NONE),
        m_BorderColor(Vector4::Zero()),
        m_MinLod(0),
        m_DoubleBuffered(descriptor.RenderTarget)
{
    Profiler::Marker _("Texture::Texture");

    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        if (i == 0 || m_DoubleBuffered)
            m_Texture[i] = GraphicsBackend::Current()->CreateTexture(m_Width, m_Height, m_Depth, m_TextureType, descriptor.Format, m_MipLevels, descriptor.Linear, descriptor.RenderTarget, name + "_0");
    }
    RecreateSampler(false);
}

Texture::~Texture()
{
    for (int i = 0; i < GraphicsBackend::GetMaxFramesInFlight(); ++i)
    {
        if (i == 0 || m_DoubleBuffered)
            GraphicsBackend::Current()->DeleteTexture(m_Texture[i]);
    }
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

void Texture::SetFilteringMode(TextureFilteringMode mode)
{
    m_FilteringMode = mode;
    RecreateSampler(true);
}

void Texture::SetComparisonFunction(ComparisonFunction function)
{
    m_ComparisonFunction = function;
    RecreateSampler(true);
}

void Texture::UploadPixels(const void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace) const
{
    Profiler::Marker _("Texture::UploadPixels");

    const int sizeMultiplier = 1 << mipLevel;
    const unsigned int width = m_Width / sizeMultiplier;
    const unsigned int height = m_Height / sizeMultiplier;

    GraphicsBackend::Current()->UploadImagePixels(GetBackendTexture(), mipLevel, cubemapFace, width, height, depth, size, pixels);
}

void Texture::RecreateSampler(bool deleteOld)
{
    if (deleteOld)
        GraphicsBackend::Current()->DeleteSampler(m_Sampler);

    m_Sampler = GraphicsBackend::Current()->CreateSampler(m_WrapMode, m_FilteringMode, &m_BorderColor.x, m_MinLod, m_ComparisonFunction, m_SamplerName);
}
