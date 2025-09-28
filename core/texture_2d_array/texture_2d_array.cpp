#include "texture_2d_array.h"

Texture2DArray::Texture2DArray(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, bool isLinear, const std::string& name) :
        Texture(TextureType::TEXTURE_2D_ARRAY, format, width, height, depth, mipLevels, isLinear, true, name)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::Create(uint32_t width, uint32_t height, uint32_t slices, TextureInternalFormat format, bool isLinear, const std::string& name)
{
    return std::shared_ptr<Texture2DArray>(new Texture2DArray(format, width, height, slices, 1, isLinear, name));
}
