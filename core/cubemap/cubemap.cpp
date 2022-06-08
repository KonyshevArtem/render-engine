#include "cubemap.h"
#include "core_debug/debug.h"
#include "lodepng.h"
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
    auto cubemap = std::shared_ptr<Cubemap>(new Cubemap());

    cubemap->m_Data.resize(SIDES_COUNT);
    std::filesystem::path paths[SIDES_COUNT] {_xPositivePath, _xNegativePath, _yPositivePath, _yNegativePath, _zPositivePath, _zNegativePath};

    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        auto path  = (Utils::GetExecutableDirectory() / paths[i]).string();
        auto error = lodepng::decode(cubemap->m_Data[i], cubemap->m_Width, cubemap->m_Height, path, LCT_RGB);
        if (error != 0)
        {
            Debug::LogErrorFormat("[Cubemap] Error loading texture: %1%\n%2%", {lodepng_error_text(error), path});
            return nullptr;
        }
    }

    cubemap->Init();
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
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        CHECK_GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, &m_Data[i][0])); // NOLINT(cppcoreguidelines-narrowing-conversions)
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

    white->m_Data.resize(SIDES_COUNT);
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        white->m_Data[i].push_back(255);
        white->m_Data[i].push_back(255);
        white->m_Data[i].push_back(255);
    }

    white->Init();
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

    black->m_Data.resize(SIDES_COUNT);
    for (int i = 0; i < SIDES_COUNT; ++i)
    {
        black->m_Data[i].push_back(0);
        black->m_Data[i].push_back(0);
        black->m_Data[i].push_back(0);
    }

    black->Init();
    return black;
}
