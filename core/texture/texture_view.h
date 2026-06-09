#ifndef RENDER_ENGINE_TEXTURE_VIEW_H
#define RENDER_ENGINE_TEXTURE_VIEW_H

#include "graphics_backend_api.h"
#include "texture.h"
#include "types/graphics_backend_texture_view.h"
#include "types/graphics_backend_texture_view_descriptor.h"

#include <string>

class TextureView
{
public:
    TextureView(const std::shared_ptr<Texture>& texture, const GraphicsBackendTextureViewDescriptor& descriptor, const std::string& name);
    ~TextureView();

    const GraphicsBackendTextureView& GetBackendTextureView() const
    {
        return m_TextureView;
    }

    const GraphicsBackendTextureViewDescriptor& GetDescriptor() const
    {
        return m_TextureViewDescriptor;
    }

    std::shared_ptr<Texture> GetTexture() const
    {
        return m_Texture.expired() ? nullptr : m_Texture.lock();
    }

private:
    GraphicsBackendTextureView m_TextureView;
    GraphicsBackendTextureViewDescriptor m_TextureViewDescriptor;

    std::weak_ptr<Texture> m_Texture;
};

#endif //RENDER_ENGINE_TEXTURE_VIEW_H
