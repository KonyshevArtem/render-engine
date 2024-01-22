#include "texture_2d_array.h"
#include "enums/texture_target.h"
#include "enums/texture_internal_format.h"
#include "enums/texture_pixel_format.h"
#include "enums/texture_data_type.h"

Texture2DArray::Texture2DArray(unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevels) :
        Texture(TextureType::TEXTURE_2D_ARRAY, width, height, depth, mipLevels)
{
}

std::shared_ptr<Texture2DArray> Texture2DArray::ShadowMapArray(unsigned int _size, unsigned int _count)
{
    auto texture = std::shared_ptr<Texture2DArray>(new Texture2DArray(_size, _size, _count, 1));
    texture->UploadPixels(nullptr, TextureTarget::TEXTURE_2D_ARRAY, TextureInternalFormat::DEPTH_COMPONENT, TexturePixelFormat::DEPTH_COMPONENT, TextureDataType::FLOAT, 0, 0, false);
    texture->SetWrapMode(TextureWrapMode::CLAMP_TO_BORDER);
    return texture;
}
