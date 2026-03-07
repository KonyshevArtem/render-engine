#include "texture_2d_array.h"

Texture2DArray::Texture2DArray(const Descriptor& descriptor, const std::string& name) :
        Texture(TextureType::TEXTURE_2D_ARRAY, descriptor, name)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::Create(const Descriptor& descriptor, const std::string& name)
{
    return std::shared_ptr<Texture2DArray>(new Texture2DArray(descriptor, name));
}
