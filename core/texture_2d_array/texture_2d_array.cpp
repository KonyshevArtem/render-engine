#include "texture_2d_array.h"

Texture2DArray::Texture2DArray(TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels, bool isLinear) :
        Texture(TextureType::TEXTURE_2D_ARRAY, format, width, height, depth, mipLevels, isLinear, true)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(unsigned int _size, unsigned int _count)
{
    auto texture = std::shared_ptr<Texture2DArray>(new Texture2DArray(TextureInternalFormat::DEPTH_COMPONENT, _size, _size, _count, 1, true));
    texture->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    return texture;
}
