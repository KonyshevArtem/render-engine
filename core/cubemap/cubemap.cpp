#include "cubemap.h"
#include "debug.h"
#include "texture_binary_reader.h"

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <vector>

Cubemap::Cubemap(unsigned int width, unsigned int height, unsigned int mipLevels) :
        Texture(GL_TEXTURE_CUBE_MAP, width, height, 0, mipLevels)
{
}

std::shared_ptr<Cubemap> Cubemap::Load(const std::filesystem::path &path)
{
    TextureBinaryReader reader;
    if (!reader.ReadTexture(path))
    {
        return nullptr;
    }

    const auto &header = reader.GetHeader();
    if (header.Depth != SIDES_COUNT)
    {
        Debug::LogErrorFormat("Number of slices in texture file is not %1%", {std::to_string(SIDES_COUNT)});
        return nullptr;
    }

    std::shared_ptr<Cubemap> cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.Width, header.Height, header.MipCount));
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        for (int j = 0; j < header.MipCount; ++j)
        {
            auto pixels = reader.GetPixels(i, j);
            cubemap->UploadPixels(pixels.data(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, header.InternalFormat, header.Format, GL_UNSIGNED_BYTE, pixels.size(), j, header.IsCompressed);
        }
    }

    return cubemap;
}

std::shared_ptr<Cubemap> &Cubemap::White()
{
    static std::shared_ptr<Cubemap> white;

    if (white == nullptr)
    {
        unsigned char pixels[3] = {255, 255, 255};
        white = CreateDefaultCubemap(&pixels[0]);
    }

    return white;
}

std::shared_ptr<Cubemap> &Cubemap::Black()
{
    static std::shared_ptr<Cubemap> black;

    if (black == nullptr)
    {
        unsigned char pixels[3] = {0, 0, 0};
        black = CreateDefaultCubemap(&pixels[0]);
    }

    return black;
}

std::shared_ptr<Cubemap> Cubemap::CreateDefaultCubemap(unsigned char *pixels)
{
    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap(1, 1, 1));
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        cubemap->UploadPixels(pixels, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE, 0, 0, false);
    }
    return cubemap;
}
