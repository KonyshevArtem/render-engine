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

    constexpr int facesCount = static_cast<int>(CubemapFace::MAX);
    if (header.Depth != facesCount)
    {
        Debug::LogErrorFormat("Number of slices in texture file is %1%, expected %2%", {std::to_string(header.Depth), std::to_string(facesCount)});
        return nullptr;
    }

    std::shared_ptr<Cubemap> cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear));
    for (int face = 0; face < facesCount; ++face)
    {
        for (int mip = 0; mip < header.MipCount; ++mip)
        {
            auto pixels = reader.GetPixels(face, mip);
            cubemap->UploadPixels(pixels.data(), pixels.size(), 0, mip, static_cast<CubemapFace>(face));
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
    for (int face = 0; face < static_cast<int>(CubemapFace::MAX); ++face)
    {
        cubemap->UploadPixels(pixels, 0, 0, 0, static_cast<CubemapFace>(face));
    }
    return cubemap;
}
