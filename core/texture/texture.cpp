#include "texture.h"
#include "graphics_backend_api.h"
#include "editor/profiler/profiler.h"

#define DEFINE_TEXTURE(Type, Name, Linear, Size, ...) \
    static std::shared_ptr<Texture> texture; \
    if (texture == nullptr) \
    { \
        constexpr uint8_t pixels[Size] = __VA_ARGS__; \
        texture = TextureLocal::CreateTexture(Type, &pixels[0], Linear, Size, Name); \
    } \
    return texture; \

namespace TextureLocal
{
    std::shared_ptr<Texture> CreateTexture(TextureType textureType, const uint8_t* pixels, bool linear, uint8_t size, const std::string& name)
    {
        GraphicsBackendTextureDescriptor descriptor{};
		descriptor.Type = textureType;
		descriptor.Format = TextureInternalFormat::RGBA8;
        descriptor.Width = 1;
        descriptor.Height = 1;
        descriptor.MipLevels = 1;
        descriptor.Linear = linear;
		descriptor.RenderTarget = false;

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(descriptor, name);
        if (textureType == TextureType::TEXTURE_CUBEMAP)
        {
            for (int face = 0; face < static_cast<int>(CubemapFace::MAX); ++face)
                texture->UploadPixels(pixels, size, 0, 0, static_cast<CubemapFace>(face));
        }
        else
            texture->UploadPixels(pixels, size, 0, 0);

        return texture;
    }
}

Texture::Texture(const GraphicsBackendTextureDescriptor& descriptor, const std::string& name) :
        m_TextureDescriptor(descriptor),
		m_SamplerName(name + "_Sampler"),
		m_SamplerDescriptor({}),
		m_SamplerDirty(true),
		m_HasSampler(false)
{
    Profiler::Marker _("Texture::Texture");

    m_SamplerDescriptor.WrapMode = TextureWrapMode::REPEAT;
    m_SamplerDescriptor.FilteringMode = descriptor.MipLevels > 1 ? TextureFilteringMode::LINEAR_MIPMAP_NEAREST : TextureFilteringMode::LINEAR;
    m_SamplerDescriptor.HasBorderColor = true;
    m_Texture = GraphicsBackend::Current()->CreateTexture(descriptor, name);
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

std::shared_ptr<Texture> Texture::White()
{
	DEFINE_TEXTURE(TextureType::TEXTURE_2D, "White", false, 4, { 255, 255, 255, 255 })
}

std::shared_ptr<Texture> Texture::Normal()
{
    DEFINE_TEXTURE(TextureType::TEXTURE_2D, "Normal", true, 4, { 125, 125, 255, 255 })
}

std::shared_ptr<Texture> Texture::BlackCube()
{
    DEFINE_TEXTURE(TextureType::TEXTURE_CUBEMAP, "BlackCube", false, 4, { 0, 0, 0, 0 })
}

std::shared_ptr<Texture> Texture::WhiteCube()
{
	DEFINE_TEXTURE(TextureType::TEXTURE_CUBEMAP, "WhiteCube", false, 4, { 255, 255, 255, 255 })
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
