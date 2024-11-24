#include "cubemap.h"
#include "debug.h"
#include "texture/texture_binary_reader.h"

Cubemap::Cubemap(TextureInternalFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, bool isLinear, const std::string& name) :
        Texture(TextureType::TEXTURE_CUBEMAP, format, width, height, 0, mipLevels, isLinear, false, name)
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
        Debug::LogErrorFormat("Number of slices in texture file is {}, expected {}", std::to_string(header.Depth), std::to_string(facesCount));
        return nullptr;
    }

    std::shared_ptr<Cubemap> cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.TextureFormat, header.Width, header.Height, header.MipCount, header.IsLinear, path.string()));
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
    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(TextureInternalFormat::RGBA8, 1, 1, 1, false, name));
    for (int face = 0; face < static_cast<int>(CubemapFace::MAX); ++face)
    {
        cubemap->UploadPixels(pixels, size, 0, 0, static_cast<CubemapFace>(face));
    }
    return cubemap;
}
