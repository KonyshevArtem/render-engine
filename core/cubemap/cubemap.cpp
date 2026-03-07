#include "cubemap.h"

Cubemap::Cubemap(const Descriptor& descriptor, const std::string& name) :
        Texture(TextureType::TEXTURE_CUBEMAP, descriptor, name)
{
}

std::shared_ptr<Cubemap> &Cubemap::White()
{
    static std::shared_ptr<Cubemap> white;

    if (white == nullptr)
    {
        uint8_t pixels[4] = {255, 255, 255, 255};
        white = CreateDefaultCubemap(&pixels[0], 4, "WhiteCubemap");
    }

    return white;
}

std::shared_ptr<Cubemap> &Cubemap::Black()
{
    static std::shared_ptr<Cubemap> black;

    if (black == nullptr)
    {
        uint8_t pixels[4] = {0, 0, 0, 255};
        black = CreateDefaultCubemap(&pixels[0], 4, "BlackCubemap");
    }

    return black;
}

std::shared_ptr<Cubemap> Cubemap::CreateDefaultCubemap(uint8_t* pixels, uint8_t size, const std::string& name)
{
    Descriptor descriptor{};
    descriptor.Width = 1;
    descriptor.Height = 1;
    descriptor.MipLevels = 1;
    descriptor.Linear = false;

    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(descriptor, name));
    for (int face = 0; face < static_cast<int>(CubemapFace::MAX); ++face)
    {
        cubemap->UploadPixels(pixels, size, 0, 0, static_cast<CubemapFace>(face));
    }
    return cubemap;
}
