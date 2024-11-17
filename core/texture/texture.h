#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include "enums/texture_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/texture_internal_format.h"
#include "enums/cubemap_face.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "vector4/vector4.h"

#include <string>

class Texture
{
public:
    virtual ~Texture();

    void SetMinMipLevel(int minMipLevel);
    void SetWrapMode(TextureWrapMode wrapMode);
    void SetBorderColor(const Vector4 &color);

    inline const GraphicsBackendTexture& GetBackendTexture() const
    {
        return m_Texture;
    }

    inline const GraphicsBackendSampler& GetBackendSampler() const
    {
        return m_Sampler;
    }

    inline uint32_t GetWidth() const
    {
        return m_Width;
    }

    inline uint32_t GetHeight() const
    {
        return m_Height;
    }

    inline uint32_t GetMipLevels() const
    {
        return m_MipLevels;
    }

    Texture(const Texture &) = delete;
    Texture(Texture &&) = delete;

    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&) = delete;

protected:
    Texture(TextureType textureType, TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, bool isLinear, bool isRenderTarget, const std::string& name);

    void UploadPixels(void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace = CubemapFace::POSITIVE_X) const;

private:
    void RecreateSampler(bool deleteOld);

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_Depth = 0;
    GraphicsBackendTexture m_Texture{};
    GraphicsBackendSampler m_Sampler{};
    TextureType m_TextureType = TextureType::TEXTURE_2D;
    uint32_t m_MipLevels = 0;
    std::string m_SamplerName;

    TextureWrapMode m_WrapMode;
    TextureFilteringMode m_FilteringMode;
    Vector4 m_BorderColor;
    int m_MinLod;
};

#endif //RENDER_ENGINE_TEXTURE_H
