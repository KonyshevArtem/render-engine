#include "cubemap.h"
#include "debug.h"
#include "texture/texture_binary_reader.h"

Cubemap::Cubemap(TextureInternalFormat format, unsigned int width, unsigned int height, unsigned int mipLevels, bool isLinear) :
        Texture(TextureType::TEXTURE_CUBEMAP, format, width, height, 0, mipLevels, isLinear, false)
{
}

std::shared_ptr<Cubemap> Cubemap::Load(const std::filesystem::path& path)
{
    std::filesystem::path fixedPath = path.parent_path() / "output" / path.filename();

    TextureBinaryReader reader;
    if (!reader.ReadTexture(fixedPath))
    {
        return nullptr;
    }

    const auto &header = reader.GetHeader();
    if (header.Depth != SIDES_COUNT)
    {
        Debug::LogErrorFormat("Number of slices in texture file is %1%, expected %2%", {std::to_string(header.Depth), std::to_string(SIDES_COUNT)});
        return nullptr;
    }

    std::shared_ptr<Cubemap> cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear));
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        for (int j = 0; j < header.MipCount; ++j)
        {
            auto pixels = reader.GetPixels(i, j);
            cubemap->UploadPixels(pixels.data(), pixels.size(), 0, j, i);
        }
    }

    return cubemap;
}

std::shared_ptr<Cubemap> &Cubemap::White()
{
    static std::shared_ptr<Cubemap> white;

    if (white == nullptr)
    {
        unsigned char pixels[4] = {255, 255, 255, 255};
        white = CreateDefaultCubemap(&pixels[0]);
    }

    return white;
}

std::shared_ptr<Cubemap> &Cubemap::Black()
{
    static std::shared_ptr<Cubemap> black;

    if (black == nullptr)
    {
        unsigned char pixels[4] = {0, 0, 0, 255};
        black = CreateDefaultCubemap(&pixels[0]);
    }

    return black;
}

std::shared_ptr<Cubemap> Cubemap::CreateDefaultCubemap(unsigned char *pixels)
{
    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(TextureInternalFormat::RGBA8, 1, 1, 1, false));
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        cubemap->UploadPixels(pixels, 0, 0, 0, 0);
    }
    return cubemap;
}
