#include "cubemap.h"
#include "debug.h"
#include "texture/texture_header.h"
#include "utils.h"
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

std::shared_ptr<Cubemap> Cubemap::Load(const std::filesystem::path &_xPositivePath,
                                       const std::filesystem::path &_xNegativePath,
                                       const std::filesystem::path &_yPositivePath,
                                       const std::filesystem::path &_yNegativePath,
                                       const std::filesystem::path &_zPositivePath,
                                       const std::filesystem::path &_zNegativePath)
{
    static constexpr unsigned int headerSize = sizeof(TextureHeader);

    std::shared_ptr<Cubemap> cubemap = nullptr;

    std::filesystem::path paths[SIDES_COUNT] {_xPositivePath, _xNegativePath, _yPositivePath, _yNegativePath, _zPositivePath, _zNegativePath};
    std::vector<char> pixels;

    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        if (!Utils::ReadFileBytes(Utils::GetExecutableDirectory() / paths[i], pixels))
        {
            break;
        }

        TextureHeader header = *reinterpret_cast<TextureHeader*>(pixels.data());
        if (i == 0)
        {
            cubemap = std::shared_ptr<Cubemap>(new Cubemap(header.Width, header.Height, header.MipCount));
        }
        else if (header.Width != cubemap->GetWidth() || header.Height != cubemap->GetHeight())
        {
            Debug::LogError("Cubemap texture sizes do not match");
            break;
        }

        auto *sizes = reinterpret_cast<unsigned int*>(&pixels[0] + sizeof(header));

        unsigned int offset = headerSize + header.MipCount * sizeof(int);
        for (int j = 0; j < header.MipCount; ++j)
        {
            cubemap->UploadPixels(pixels.data() + offset, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, header.InternalFormat, header.Format, GL_UNSIGNED_BYTE, sizes[j], j, header.IsCompressed);
            offset += sizes[j];
        }

        pixels.clear();
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
    auto white = std::shared_ptr<Cubemap>(new Cubemap(1, 1, 1));
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        white->UploadPixels(pixels, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE, 0, 0, false);
    }
    return white;
}
