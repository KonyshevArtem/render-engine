#include "texture_2d.h"

Texture2D::Texture2D(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, bool isLinear, bool isRenderTarget, const std::string& name) :
        Texture(TextureType::TEXTURE_2D, format, width, height, 0, mipLevels, isLinear, isRenderTarget, name)
{
}

std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget, const std::string& name)
{
    return std::shared_ptr<Texture2D>(new Texture2D(textureFormat, width, height, 1, isLinear, isRenderTarget, name));
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white == nullptr)
    {
        uint8_t pixels[4] {255, 255, 255, 255};
        white = std::shared_ptr<Texture2D>(new Texture2D(TextureInternalFormat::RGBA8, 1, 1, 1, false, false, "White"));
        white->UploadPixels(pixels, 4, 0, 0);
    }

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal == nullptr)
    {
        uint8_t pixels[4] {125, 125, 255, 255};
        normal = std::shared_ptr<Texture2D>(new Texture2D(TextureInternalFormat::RGBA8, 1, 1, 1, true, false, "DefaultNormal"));
        normal->UploadPixels(pixels, 4, 0, 0);
    }

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}
