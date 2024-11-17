#include "texture_2d_array.h"

Texture2DArray::Texture2DArray(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, bool isLinear, const std::string& name) :
        Texture(TextureType::TEXTURE_2D_ARRAY, format, width, height, depth, mipLevels, isLinear, true, name)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(uint32_t size, uint32_t count, const std::string& name)
{
    auto texture = std::shared_ptr<Texture2DArray>(new Texture2DArray(TextureInternalFormat::DEPTH_COMPONENT, size, size, count, 1, true, name));
    texture->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    return texture;
}
