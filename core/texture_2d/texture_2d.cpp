#include "texture_2d.h"
#include "texture/texture_binary_reader.h"

#include <vector>

Texture2D::Texture2D(unsigned int width, unsigned int height, unsigned int mipLevels) :
        Texture(TextureType::TEXTURE_2D, width, height, 0, mipLevels)
{
}

std::shared_ptr<Texture2D> Texture2D::Create(int _width, int _height)
{
    return Create_Internal(nullptr, _width, _height, TextureInternalFormat::SRGB_ALPHA, TexturePixelFormat::RGBA);
}

std::shared_ptr<Texture2D> Texture2D::CreateShadowMap(int _width, int _height)
{
    return Create_Internal(nullptr, _width, _height, TextureInternalFormat::DEPTH_COMPONENT, TexturePixelFormat::DEPTH_COMPONENT);
}

std::shared_ptr<Texture2D> Texture2D::Load(const std::filesystem::path &_path)
{
    TextureBinaryReader reader;
    if (!reader.ReadTexture(_path))
    {
        return nullptr;
    }

    const auto &header = reader.GetHeader();

    auto t = std::shared_ptr<Texture2D>(new Texture2D(header.Width, header.Height, header.MipCount));
    for (int j = 0; j < header.MipCount; ++j)
    {
        auto pixels = reader.GetPixels(0, j);
        t->UploadPixels(pixels.data(), TextureTarget::TEXTURE_2D, header.TextureFormat, header.PixelFormat, TextureDataType::UNSIGNED_BYTE, pixels.size(), j, header.IsCompressed);
    }

    return t;
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white == nullptr)
    {
        unsigned char pixels[3] {255, 255, 255};
        white = Create_Internal(&pixels[0], 1, 1, TextureInternalFormat::SRGB, TexturePixelFormat::RGB);
    }

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal == nullptr)
    {
        unsigned char pixels[3] {125, 125, 255};
        normal = Create_Internal(&pixels[0], 1, 1, TextureInternalFormat::RGB, TexturePixelFormat::RGB);
    }

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}

std::shared_ptr<Texture2D> Texture2D::Create_Internal(uint8_t *pixels, int width, int height, TextureInternalFormat textureFormat, TexturePixelFormat pixelFormat)
{
    auto texture = std::shared_ptr<Texture2D>(new Texture2D(width, height, 1));
    texture->UploadPixels(pixels, TextureTarget::TEXTURE_2D, textureFormat, pixelFormat, TextureDataType::UNSIGNED_BYTE, 0, 0, false);
    return texture;
}
