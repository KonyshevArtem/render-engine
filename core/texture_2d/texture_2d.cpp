#include "texture_2d.h"
#include "texture/texture_binary_reader.h"

#include <vector>

Texture2D::Texture2D(TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int mipLevels, bool isLinear, bool isRenderTarget) :
        Texture(TextureType::TEXTURE_2D, format, width, height, 0, mipLevels, isLinear, isRenderTarget)
{
}

std::shared_ptr<Texture2D> Texture2D::Create(int _width, int _height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget)
{
    return Create_Internal(nullptr, _width, _height, textureFormat, isLinear, isRenderTarget);
}

std::shared_ptr<Texture2D> Texture2D::Load(const std::filesystem::path& path)
{
    std::filesystem::path fixedPath = path.parent_path() / "output" / path.filename();

    TextureBinaryReader reader;
    if (!reader.ReadTexture(fixedPath))
    {
        return nullptr;
    }

    const auto &header = reader.GetHeader();

    auto t = std::shared_ptr<Texture2D>(new Texture2D(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, false));
    for (int mip = 0; mip < header.MipCount; ++mip)
    {
        auto pixels = reader.GetPixels(0, mip);
        t->UploadPixels(pixels.data(), pixels.size(), 0, mip);
    }

    return t;
}

const std::shared_ptr<Texture2D> &Texture2D::White()
{
    static std::shared_ptr<Texture2D> white;

    if (white == nullptr)
    {
        unsigned char pixels[4] {255, 255, 255, 255};
        white = Create_Internal(&pixels[0], 1, 1, TextureInternalFormat::RGBA8, false, false);
    }

    return white;
}

const std::shared_ptr<Texture2D> &Texture2D::Normal()
{
    static std::shared_ptr<Texture2D> normal;

    if (normal == nullptr)
    {
        unsigned char pixels[4] {125, 125, 255, 255};
        normal = Create_Internal(&pixels[0], 1, 1, TextureInternalFormat::RGBA8, false, false);
    }

    return normal;
}

const std::shared_ptr<Texture2D> &Texture2D::Null()
{
    static std::shared_ptr<Texture2D> null;
    return null;
}

std::shared_ptr<Texture2D> Texture2D::Create_Internal(uint8_t *pixels, int width, int height, TextureInternalFormat textureFormat, bool isLinear, bool isRenderTarget)
{
    auto texture = std::shared_ptr<Texture2D>(new Texture2D(textureFormat, width, height, 1, isLinear, isRenderTarget));
    if (!isRenderTarget)
    {
        texture->UploadPixels(pixels, 0, 0, 0);
    }
    return texture;
}
