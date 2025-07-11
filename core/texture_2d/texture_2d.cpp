#include "texture_2d.h"

Texture2D::Texture2D(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, bool isLinear, bool isRenderTarget, const std::string& name) :
        Texture(TextureType::TEXTURE_2D, format, width, height, 0, mipLevels, isLinear, isRenderTarget, name)
{
}

std::shared_ptr<Texture2D> Texture2D::Create(uint32_t _width, uint32_t _height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget, const std::string& name)
{
    return Create_Internal(nullptr, 0, _width, _height, textureFormat, isLinear, isRenderTarget, name);
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white == nullptr)
    {
        uint8_t pixels[4] {255, 255, 255, 255};
        white = Create_Internal(&pixels[0], 4, 1, 1, TextureInternalFormat::RGBA8, false, false, "White");
    }

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal == nullptr)
    {
        uint8_t pixels[4] {125, 125, 255, 255};
        normal = Create_Internal(&pixels[0], 4, 1, 1, TextureInternalFormat::RGBA8, true, false, "DefaultNormal");
    }

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}

std::shared_ptr<Texture2D> Texture2D::Create_Internal(uint8_t *pixels, uint8_t size, uint32_t width, uint32_t height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget, const std::string& name)
{
    auto texture = std::shared_ptr<Texture2D>(new Texture2D(textureFormat, width, height, 1, isLinear, isRenderTarget, name));
    if (!isRenderTarget)
    {
        texture->UploadPixels(pixels, size, 0, 0);
    }
    return texture;
}
