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
    Texture(const GraphicsBackendTextureDescriptor& descriptor, const std::string& name);
    virtual ~Texture();

    void SetMinMipLevel(int minMipLevel);
    void SetWrapMode(TextureWrapMode wrapMode);
    void SetBorderColor(const Vector4 &color);
    void SetFilteringMode(TextureFilteringMode mode);
    void SetComparisonFunction(ComparisonFunction function);

    const GraphicsBackendTexture& GetBackendTexture() const
    {
        return m_Texture;
    }

    const GraphicsBackendSampler& GetBackendSampler();

    uint32_t GetWidth() const
    {
        return m_TextureDescriptor.Width;
    }

    uint32_t GetHeight() const
    {
        return m_TextureDescriptor.Height;
    }

    uint32_t GetMipLevels() const
    {
        return m_TextureDescriptor.MipLevels;
    }

	const GraphicsBackendTextureDescriptor& GetTextureDescriptor() const
    {
        return m_TextureDescriptor;
    }

    void UploadPixels(const void *pixels, int size, int depth, int mipLevel, CubemapFace cubemapFace = CubemapFace::POSITIVE_X) const;

    static std::shared_ptr<Texture> White();
    static std::shared_ptr<Texture> Normal();
    static std::shared_ptr<Texture> BlackCube();
    static std::shared_ptr<Texture> WhiteCube();

private:
    void RecreateSampler();

    GraphicsBackendTextureDescriptor m_TextureDescriptor;
    GraphicsBackendTexture m_Texture;
    GraphicsBackendSampler m_Sampler{};
    std::string m_SamplerName;

    GraphicsBackendSamplerDescriptor m_SamplerDescriptor;
    bool m_SamplerDirty;
    bool m_HasSampler;

    friend class Resources;
    friend class Font;
};

#endif //RENDER_ENGINE_TEXTURE_H
