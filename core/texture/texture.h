#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include "graphics_backend_api.h"
#include "enums/texture_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/texture_internal_format.h"
#include "enums/cubemap_face.h"
#include "enums/comparison_function.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_texture_descriptor.h"
#include "types/graphics_backend_sampler_descriptor.h"
#include "vector4/vector4.h"
#include "resources/resource.h"

#include <string>

class Texture : public Resource
{
public:
    virtual ~Texture();

    void SetMinMipLevel(int minMipLevel);
    void SetWrapMode(TextureWrapMode wrapMode);
    void SetBorderColor(const Vector4 &color);
    void SetFilteringMode(TextureFilteringMode mode);
    void SetComparisonFunction(ComparisonFunction function);

    inline const GraphicsBackendTexture& GetBackendTexture() const
    {
        return m_Texture;
    }

    const GraphicsBackendSampler& GetBackendSampler();

    inline uint32_t GetWidth() const
    {
        return m_TextureDescriptor.Width;
    }

    inline uint32_t GetHeight() const
    {
        return m_TextureDescriptor.Height;
    }

    inline uint32_t GetMipLevels() const
    {
        return m_TextureDescriptor.MipLevels;
    }

    Texture(const Texture &) = delete;
    Texture(Texture &&) = delete;

    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&) = delete;

protected:
    Texture(TextureType textureType, const GraphicsBackendTextureDescriptor& descriptor, const std::string& name);

    void UploadPixels(const void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace = CubemapFace::POSITIVE_X) const;

private:
    void RecreateSampler();

    GraphicsBackendTextureDescriptor m_TextureDescriptor;
    GraphicsBackendTexture m_Texture;
    GraphicsBackendSampler m_Sampler{};
    TextureType m_TextureType = TextureType::TEXTURE_2D;
    std::string m_SamplerName;

    GraphicsBackendSamplerDescriptor m_SamplerDescriptor;
    bool m_SamplerDirty;
    bool m_HasSampler;

    friend class Resources;
    friend class Font;
};

#endif //RENDER_ENGINE_TEXTURE_H
