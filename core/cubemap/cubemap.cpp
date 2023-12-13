#include "cubemap.h"
#include "debug.h"
#include "texture/texture_header.h"
#include "utils.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

std::shared_ptr<Cubemap> Cubemap::Load(const std::filesystem::path &_xPositivePath,
                                       const std::filesystem::path &_xNegativePath,
                                       const std::filesystem::path &_yPositivePath,
                                       const std::filesystem::path &_yNegativePath,
                                       const std::filesystem::path &_zPositivePath,
                                       const std::filesystem::path &_zNegativePath)
{
    static constexpr int headerSize = sizeof(TextureHeader);

    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap());
    cubemap->Init();

    std::filesystem::path paths[SIDES_COUNT] {_xPositivePath, _xNegativePath, _yPositivePath, _yNegativePath, _zPositivePath, _zNegativePath};
    std::vector<unsigned char> pixels;

    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        std::vector<char> pixels = Utils::ReadFileBytes(Utils::GetExecutableDirectory() / paths[i]);

        TextureHeader header = *reinterpret_cast<TextureHeader*>(pixels.data());
        cubemap->m_Width = header.Width;
        cubemap->m_Height = header.Height;

        cubemap->UploadPixels(pixels.data() + headerSize, i, header.InternalFormat, header.Format, pixels.size() * sizeof(char) - headerSize, true);
        pixels.clear();
    }

    return cubemap;
}

void Cubemap::Init()
{
    CHECK_GL(glGenTextures(1, &m_Texture));
    CHECK_GL(glGenSamplers(1, &m_Sampler));
    CHECK_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, GL_REPEAT));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK_GL(glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void Cubemap::UploadPixels(void *_pixels, int side, int _internalFormat, int _format, int _size, bool _compressed)
{
    CHECK_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture));
    if (_compressed)
    {
        CHECK_GL(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, _internalFormat, m_Width, m_Height, 0, _size, _pixels)); // NOLINT(cppcoreguidelines-narrowing-conversions)
    }
    else
    {
        CHECK_GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, _internalFormat, m_Width, m_Height, 0, _format, GL_UNSIGNED_BYTE, _pixels));
    }
    CHECK_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void Cubemap::Bind(int _unit) const
{
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + _unit));
    CHECK_GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture));
    CHECK_GL(glBindSampler(_unit, m_Sampler));
}

void Cubemap::Attach(int _attachment) const
{
    Debug::LogError("Attaching cubemap is not supported");
}

std::shared_ptr<Cubemap> &Cubemap::White()
{
    static std::shared_ptr<Cubemap> white;

    if (white != nullptr)
        return white;

    white           = std::shared_ptr<Cubemap>(new Cubemap());
    white->m_Width  = 1;
    white->m_Height = 1;
    white->Init();

    unsigned char pixels[3] = {255, 255, 255};
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        white->UploadPixels(&pixels[0], i, GL_SRGB, GL_RGB, 0, false);
    }

    return white;
}

std::shared_ptr<Cubemap> &Cubemap::Black()
{
    static std::shared_ptr<Cubemap> black;

    if (black != nullptr)
        return black;

    black           = std::shared_ptr<Cubemap>(new Cubemap());
    black->m_Width  = 1;
    black->m_Height = 1;
    black->Init();

    unsigned char pixels[3] = {0, 0, 0};
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        black->UploadPixels(&pixels[0], i, GL_SRGB, GL_RGB, 0, false);
    }

    return black;
}
